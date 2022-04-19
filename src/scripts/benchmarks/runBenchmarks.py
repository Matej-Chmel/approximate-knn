if __package__ is None:
	from pathlib import Path
	import subprocess
	import sys

	subprocess.call(
		[sys.executable, "-m", "src.scripts.benchmarks.runBenchmarks", *sys.argv[1:]],
		cwd=Path(__file__).parents[3]
	)
	sys.exit(0)

from argparse import ArgumentParser, Namespace
import pandas
from pathlib import Path
import time
from src.benchmarks.ann_benchmarks.datasets import DATASETS
import subprocess
import sys
import webbrowser as wb

SRC_DIR = Path(__file__).parents[2]
BENCHMARKS_DIR = SRC_DIR / "benchmarks"
CONFIG_DIR = SRC_DIR / "config"
DEFAULT_ALGOS_PATH = BENCHMARKS_DIR / "algos.yaml"
DEFAULT_DATASETS_PATH = CONFIG_DIR / "datasets.txt"
N = "\n"
SCRIPT_DIR = Path(__file__).parent

class AppError(Exception):
	pass

class Config:
	def __init__(self):
		self.algoDefPaths = [DEFAULT_ALGOS_PATH]
		self.datasets = None
		self.datasetsPath = DEFAULT_DATASETS_PATH
		self.dockerWorkers = 1
		self.force = False
		self.runs = 1

	def __str__(self):
		return (
			f"Algorithm definitions: {', '.join(map(str, self.algoDefPaths))}{N}"
			f"Datasets: {'NOT PARSED' if self.datasets is None else ', '.join(self.datasets)}{N}"
			f"Datasets file: {'NONE' if self.datasetsPath is None else self.datasetsPath}{N}"
			f"Docker workers: {self.dockerWorkers}{N}"
			f"Force re-run: {self.force}{N}"
			f"Runs: {self.runs}"
		)

	def checkDatasetsForErrors(self):
		for d in self.datasets:
			if d not in DATASETS:
				raise AppError(f'Dataset "{d}" is not a valid dataset.')
		return self

	def checkForErrors(self):
		for a in self.algoDefPaths:
			if not a.exists():
				raise FileNotFoundError(f"Algorithm definitions file {a} does not exist.")

		if self.datasets is None and not self.datasetsPath.exists():
			raise FileNotFoundError(f"Datasets file {self.datasetsPath} does not exist.")

		return self

	def parseDatasetsFile(self):
		if self.datasetsPath is not None:
			self.datasets = parseDatasetsFile(self.datasetsPath)
		return self

	def setAlgoDefPaths(self, algoDefPaths: list[str]):
		self.algoDefPaths = [Path(a).absolute() for a in algoDefPaths]
		return self

	def setDatasets(self, datasets: list[str]):
		self.datasets = datasets
		self.datasetsPath = None
		return self

	def setDatasetsPath(self, datasetsPath: str):
		self.datasets = None
		self.datasetsPath = Path(datasetsPath).absolute()
		return self

	def setDockerWorkers(self, workers: int):
		self.dockerWorkers = workers
		return self

	def setForce(self, force: bool):
		self.force = force
		return self

	def setRuns(self, runs: int):
		self.runs = runs
		return self

	@classmethod
	def fromArgs(cls, args: Namespace):
		cfg = cls().setAlgoDefPaths(args.algoDefPaths).setDockerWorkers(args.workers
			).setForce(args.force).setRuns(args.runs)

		if args.datasets:
			cfg.setDatasets(args.datasets)
		else:
			cfg.setDatasetsPath(args.datasetsPath)

		return cfg

def createWebsite(websiteDir: Path):
	print("Creating website.")
	websiteDir.mkdir(exist_ok=True)
	subprocess.call([
		sys.executable, "create_website.py", "--latex", "--outputdir", websiteDir.absolute()
	], cwd=BENCHMARKS_DIR)
	print("Website created.")

def getArgs():
	p = ArgumentParser("BENCHMARK_MULTIPLE", description="Runs multiple benchmarks.")
	p.add_argument(
		"-a", "--algoDefPaths", help="Paths to YAML files with algorithm definitions.",
		nargs="+", required=True
	)
	p.add_argument(
		"-d", "--datasets", help="Names of datasets to benchmark on.", nargs="*"
	)
	p.add_argument(
		"-f", "--force", action="store_true", help="Force re-running already computed benchmarks."
	)
	p.add_argument(
		"-p", "--datasetsPath", default=DEFAULT_DATASETS_PATH, help="Path to text file with datasets."
	)
	p.add_argument("-r", "--runs", default=1, help="Number of runs per benchmark.", type=int)
	p.add_argument("-w", "--workers", default=1, help="Number of Docker workers.", type=int)
	return p.parse_args()

def installDockerImages():
	print("Installing Docker images.")
	subprocess.call([sys.executable, "install.py"], cwd=BENCHMARKS_DIR)
	print("Docker images installed.")

def openWebsite(websiteDir: Path):
	wb.open_new_tab(f"file:///{(websiteDir / 'index.html').absolute()}")

def parseDatasetsFile(p: Path):
	res = []

	with p.open("r", encoding="utf-8") as f:
		for line in f:
			line = line.strip()

			if not line.startswith("#") and line:
				res.append(line)

	return res

def run(cfg: Config, websiteDir: Path):
	installDockerImages()
	start = time.perf_counter_ns()
	runBenchmarks(cfg)
	createWebsite(websiteDir)
	end = time.perf_counter_ns()

	with (SCRIPT_DIR / "benchmarks_time.txt").open("w", encoding="utf-8") as f:
		diff = end - start
		f.write(f"[{pandas.Timedelta(nanoseconds=diff)}], {diff} ns{N}")

def runBenchmarks(cfg: Config):
	print("Running benchmarks.")
	print(cfg)

	for algoDefPath in cfg.algoDefPaths:
		for dataset in cfg.datasets:
			runDataset(algoDefPath, dataset, cfg)

	print("Benchmarks completed.")

def runDataset(algoDefPath: Path, dataset: str, cfg: Config):
	print(f"Running benchmarks for dataset {dataset}.")
	subprocess.call([
		sys.executable, "run.py", "--dataset", dataset, "--definitions", str(algoDefPath),
		"--parallelism", str(cfg.dockerWorkers), "--runs", str(cfg.runs)
	] + (["--force"] if cfg.force else []), cwd=BENCHMARKS_DIR)
	print(f"Benchmarks for dataset {dataset} completed.")

def tryRun(cfg: Config):
	try:
		websiteDir = SRC_DIR / "website"
		run(cfg.checkForErrors().parseDatasetsFile().checkDatasetsForErrors(), websiteDir)
		openWebsite(websiteDir)
	except AppError as e:
		print(f"[APP ERROR] {e}")
	except FileNotFoundError as e:
		print(f"[FILE NOT FOUND] {e}")
	except subprocess.SubprocessError as e:
		print(f"[SUBPROCESS ERROR] {e}")

def main():
	tryRun(Config.fromArgs(getArgs()))

if __name__ == "__main__":
	main()
