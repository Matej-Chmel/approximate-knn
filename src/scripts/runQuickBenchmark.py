if __package__ is None:
	from chmDataset import runner
	from pathlib import Path
	import subprocess
	import sys

	runner.run(lambda: subprocess.check_call(
		[sys.executable, "-m", "src.scripts.runQuickBenchmark", *sys.argv[1:]],
		cwd=Path(__file__).parents[2]
	))

from argparse import ArgumentParser
from src.scripts.chmDataset import runner
import src.scripts.runBenchmarks as r

def main():
	p = ArgumentParser("QUICK_BENCHMARK", description="Runs a quick benchmark.")
	p.add_argument("-a", "--angular", action="store_true", help="Run benchmark on an angular dataset.")
	args = p.parse_args()
	r.openWebsiteAfter(r.Config(
		).setAlgoDefPaths([r.CONFIG_DIR / "quick.yaml"]
		).setDatasets(["lastfm-64-dot" if args.angular else "mnist-784-euclidean"]
		).setDockerWorkers(2
		).setRuns(1)
	)

if __name__ == "__main__":
	runner.run(main)
