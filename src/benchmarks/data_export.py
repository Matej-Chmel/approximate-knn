from argparse import ArgumentParser
import csv
from dataclasses import dataclass
from pathlib import Path
from ann_benchmarks.datasets import DATASETS, get_dataset
from ann_benchmarks.plotting.utils  import compute_metrics_all_runs
from ann_benchmarks.results import load_all_results

@dataclass
class Args:
	outputPath: Path
	recompute: bool

def exportData(a: Args):
	datasets = DATASETS.keys()
	dfs = []

	for dataset_name in datasets:
		print("Looking at dataset", dataset_name)

		if len(list(load_all_results(dataset_name))) > 0:
			results = load_all_results(dataset_name)
			dataset, _ = get_dataset(dataset_name)
			results = compute_metrics_all_runs(dataset, results, a.recompute)

			for res in results:
				res["dataset"] = dataset_name
				dfs.append(res)

	if len(dfs) > 0:
		with a.outputPath.open("w", encoding="utf-8", newline="") as csvfile:
			names = list(dfs[0].keys())
			writer = csv.DictWriter(csvfile, fieldnames=names)
			writer.writeheader()

			for res in dfs:
				writer.writerow(res)

def getArgs():
	p = ArgumentParser()
	p.add_argument("-o", "--output", help="Path to the output file", required=True)
	p.add_argument("-r", "--recompute", action="store_true", help="Recompute metrics")
	args = p.parse_args()
	return Args(Path(args.output), args.recompute)

def main():
	exportData(getArgs())

if __name__ == "__main__":
	main()
