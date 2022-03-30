from dataclasses import dataclass
import pandas
from pathlib import Path
import time
import subprocess
import sys

N = "\n"

class Config:
	def __init__(self, algosPath: Path, datasetsPath: Path, dockerWorkers: int, runs: int):
		self.algosPath = str(algosPath.absolute())
		self.datasets = parseDatasetsFile(datasetsPath)
		self.dockerWorkers = str(dockerWorkers)
		self.runs = str(runs)

def createWebsite(scriptDir: Path):
	print("Creating website.")
	subprocess.call([sys.executable, "create_website.py", "--latex", "--outputdir", "website/"], cwd=scriptDir)
	print("Website created.")

def installDockerImages(scriptDir: Path):
	print("Installing Docker images.")
	subprocess.call([sys.executable, "install.py"], cwd=scriptDir)
	print("Docker images installed.")

def parseDatasetsFile(p: Path):
	res = []

	with p.open("r", encoding="utf-8") as f:
		for line in f:
			line = line.strip()

			if not line.startswith("#") and line:
				res.append(line)

	return res

def run(cfg: Config):
	scriptDir = Path(__file__).parent

	installDockerImages(scriptDir)
	start = time.perf_counter_ns()
	runBenchmarks(cfg, scriptDir)
	createWebsite(scriptDir)
	end = time.perf_counter_ns()

	with (scriptDir / "benchmarks_time.txt").open("w", encoding="utf-8") as f:
		diff = end - start
		f.write(f"[{pandas.Timedelta(nanoseconds=diff)}], {diff} ns{N}")

def runBenchmarks(cfg: Config, scriptDir: Path):
	print("Running benchmarks.")

	for d in cfg.datasets:
		runDataset(d, cfg, scriptDir)

	print("Benchmarks completed.")

def runDataset(dataset: str, cfg: Config, scriptDir: Path):
	print(f"Running benchmarks for dataset {dataset}.")
	subprocess.call([
		sys.executable, "run.py", "--dataset", dataset, "--definitions", cfg.algosPath,
		"--force", "--parallelism", cfg.dockerWorkers, "--runs", cfg.runs
	], cwd=scriptDir)
	print(f"Benchmarks for dataset {dataset} completed.")

def tryRun(cfg: Config):
	try:
		run(cfg)
	except subprocess.SubprocessError as e:
		print(f"[SUBPROCESS ERROR] {e}")

if __name__ == "__main__":
	scriptDir = Path(__file__).parent
	tryRun(Config(scriptDir / "algos.yaml", scriptDir / "datasets.txt", 1, 5))