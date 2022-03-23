import pandas
from pathlib import Path
import time
import subprocess
import sys

# Datasets sorted by size.
DATASETS = [
	"lastfm-64-dot", # 19 005 025
	"glove-25-angular", # 29 587 850
	"mnist-784-euclidean", # 47 040 000
	"fashion-mnist-784-euclidean", # 47 040 000
	"glove-50-angular", # 59 175 700
	"nytimes-256-angular", # 74 240 000
	"glove-100-angular", # 88 763 550
	"glove-200-angular", # 118 351 400
	"sift-128-euclidean" # 128 000 000
]
DOCKER_WORKERS = 2
N = "\n"
RUNS = 1

def createWebsite(scriptDir: Path):
	print("Creating website.")
	subprocess.call([sys.executable, "create_website.py", "--latex", "--outputdir", "website/"], cwd=scriptDir)
	print("Website created.")

def installDockerImages(scriptDir: Path):
	print("Installing Docker images.")
	subprocess.call([sys.executable, "install.py"], cwd=scriptDir)
	print("Docker images installed.")

def run():
	scriptDir = Path(__file__).parent

	installDockerImages(scriptDir)
	start = time.perf_counter_ns()
	runBenchmarks(DATASETS, scriptDir)
	createWebsite(scriptDir)
	end = time.perf_counter_ns()

	with (scriptDir / "benchmarks_time.txt").open("w", encoding="utf-8") as f:
		diff = end - start
		f.write(f"[{pandas.Timedelta(nanoseconds=diff)}], {diff} ns{N}")

def runBenchmarks(datasets: list[str], scriptDir: Path):
	print("Running benchmarks.")

	for d in datasets:
		runDataset(d, scriptDir)

	print("Benchmarks completed.")

def runDataset(d: str, scriptDir: Path):
	print(f"Running benchmarks for dataset {d}.")
	subprocess.call([
		sys.executable, "run.py", "--dataset", d, "--force",
		"--parallelism", str(DOCKER_WORKERS), "--runs", str(RUNS)
	], cwd=scriptDir)
	print(f"Benchmarks for dataset {d} completed.")

def main():
	try:
		run()
	except subprocess.SubprocessError as e:
		print(f"[SUBPROCESS ERROR] {e}")

if __name__ == "__main__":
	main()
