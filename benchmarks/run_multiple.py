from argparse import ArgumentParser, Namespace
import pandas
from pathlib import Path
import time
import subprocess
import sys
import webbrowser as wb

SCRIPT_DIR = Path(__file__).parent
CONFIG_DIR = SCRIPT_DIR / "config"
DEFAULT_ALGOS_PATH = SCRIPT_DIR / "algos.yaml"
DEFAULT_DATASETS_PATH = CONFIG_DIR / "datasets.txt"
N = "\n"

class Config:
	def __init__(
		self, algos: list[Path], datasets: list[str], dockerWorkers: int = 1,
		force: bool = False, runs: int = 1
	):
		self.algos = [Path(a).absolute() for a in algos]
		self.datasets = datasets
		self.dockerWorkers = str(dockerWorkers)
		self.force = force
		self.runs = str(runs)

		for a in self.algos:
			if not a.exists():
				raise FileNotFoundError(f"Algorithm definitions file {a} does not exist.")

	def __str__(self):
		return (
			f"Algos: {', '.join(map(str, self.algos))}{N}"
			f"Datasets: {', '.join(self.datasets)}{N}"
			f"Docker workers: {self.dockerWorkers}{N}"
			f"Force re-run: {self.force}{N}"
			f"Runs: {self.runs}"
		)

	@classmethod
	def fromArgs(cls, args: Namespace):
		return cls.fromDatasetsPath(args.algos, args.datasets, args.workers, args.force, args.runs)

	@classmethod
	def fromDatasetsPath(
		cls, algosPath: Path, datasetsPath: Path, dockerWorkers: int = 1,
		force: bool = False, runs: int = 1
	):
		return cls(algosPath, parseDatasetsFile(datasetsPath), dockerWorkers, force, runs)

def createWebsite():
	print("Creating website.")
	subprocess.call([
		sys.executable, "create_website.py", "--latex", "--outputdir", "website/"
	], cwd=SCRIPT_DIR)
	print("Website created.")

def getArgs():
	p = ArgumentParser("BENCHMARK_MULTIPLE", description="Runs multiple benchmarks.")
	p.add_argument(
		"-a", "--algos", help="Paths to YAML files with algorithm definitions.",
		nargs="+", required=True
	)
	p.add_argument(
		"-d", "--datasets", default=DEFAULT_DATASETS_PATH, help="Path to text file with datasets."
	)
	p.add_argument(
		"-f", "--force", action="store_true", help="Force re-running already computed benchmarks."
	)
	p.add_argument("-r", "--runs", default=3, help="Number of runs per benchmark.", type=int)
	p.add_argument("-w", "--workers", default=1, help="Number of Docker workers.", type=int)
	return p.parse_args()

def installDockerImages():
	print("Installing Docker images.")
	subprocess.call([sys.executable, "install.py"], cwd=SCRIPT_DIR)
	print("Docker images installed.")

def openWebsite():
	wb.open_new_tab(f"file:///{SCRIPT_DIR.absolute()}/website/index.html")

def parseDatasetsFile(p: Path):
	res = []

	with p.open("r", encoding="utf-8") as f:
		for line in f:
			line = line.strip()

			if not line.startswith("#") and line:
				res.append(line)

	return res

def run(cfg: Config):
	installDockerImages()
	start = time.perf_counter_ns()
	runBenchmarks(cfg)
	createWebsite()
	end = time.perf_counter_ns()

	with (SCRIPT_DIR / "benchmarks_time.txt").open("w", encoding="utf-8") as f:
		diff = end - start
		f.write(f"[{pandas.Timedelta(nanoseconds=diff)}], {diff} ns{N}")

def runBenchmarks(cfg: Config):
	print("Running benchmarks.")
	print(cfg)

	for a in cfg.algos:
		for d in cfg.datasets:
			runDataset(a, d, cfg)

	print("Benchmarks completed.")

def runDataset(algos: str, dataset: str, cfg: Config):
	print(f"Running benchmarks for dataset {dataset}.")
	subprocess.call([
		sys.executable, "run.py", "--dataset", dataset, "--definitions", algos,
		"--parallelism", cfg.dockerWorkers, "--runs", cfg.runs
	] + (["--force"] if cfg.force else []), cwd=SCRIPT_DIR)
	print(f"Benchmarks for dataset {dataset} completed.")

def tryRun(cfg: Config):
	try:
		run(cfg)
		openWebsite()
	except FileNotFoundError as e:
		print(f"[FILE NOT FOUND] {e}")
	except subprocess.SubprocessError as e:
		print(f"[SUBPROCESS ERROR] {e}")

def main():
	tryRun(Config.fromArgs(getArgs()))

if __name__ == "__main__":
	main()
