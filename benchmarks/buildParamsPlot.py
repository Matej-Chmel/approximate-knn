from dataclasses import dataclass, field
from io import TextIOWrapper
import pandas as pd
import parse
from pathlib import Path

BACK_SLASH = "\\"
N = "\n"
SHORT_TO_LONG_DATASET_NAMES = {
	"glove-50-angular": "GloVe-50",
	"nytimes-256-angular": "NYTimes",
	"mnist-784-euclidean": "MNIST",
	"sift-128-euclidean": "SIFT"
}
TAB = "\t"

@dataclass
class PlotValue:
	efConstruction: int
	mMax: int
	val: float

	def getParamsHash(self):
		return self.efConstruction + self.mMax

	def getPlotTuple(self):
		return (self.getParamsHash(), self.val)

@dataclass
class Algorithm:
	name: str
	plotValues: list[PlotValue] = field(default_factory=list)

@dataclass
class Dataset:
	name: str
	algos: list[Algorithm] = field(default_factory=list)

	def getGroupPlot(self):
		res = f"{BACK_SLASH}nextgroupplot[title={self.name}]{N}"

		for algo in self.algos:
			res += f"{BACK_SLASH}addplot coordinates {{{N}% {algo.name}{N}"

			for val in sorted(algo.plotValues, key=lambda v: v.getParamsHash()):
				res += f"{TAB}{val.getPlotTuple()}{N}"

			res += f"}};{N}"

		return res

def getDataset(df: pd.DataFrame, dataset: str, algos: list[str], column: str):
	try:
		shortName = SHORT_TO_LONG_DATASET_NAMES[dataset]
	except KeyError:
		print(f"Dataset {dataset} doesn't have a short name assigned to it.")

	res = Dataset(shortName)
	df = df[df["dataset"] == dataset]

	for algo in algos:
		algoRes = Algorithm(algo)

		for _, row in df[df["algorithm"] == algo].drop_duplicates(["parameters"]).iterrows():
			algoRes.plotValues.append(getPlotValue(row["parameters"], row[column]))

		res.algos.append(algoRes)

	return res

def getPlotValue(params: str, val: float):
	parseRes = (
		parse.parse("chm(e={},m={},d={},n={},p={})", params) if params.startswith("chm")
		else parse.parse("hnswlib(e={},m={})", params)
	)
	return PlotValue(int(parseRes[0]), int(parseRes[1]), val)

def printGroupPlots(df: pd.DataFrame, algos: list[str], column: str, f: TextIOWrapper):
	print(column, file=f)
	print(N.join(
		getDataset(df, dataset, algos, column).getGroupPlot()
		for dataset in SHORT_TO_LONG_DATASET_NAMES
	), file=f)

def main():
	scriptDir = Path(__file__).parent
	algos = ["chm-hnsw-prefetching", "hnswlib"]
	df = pd.read_excel(scriptDir / "table.xlsx", sheet_name="table")

	with (scriptDir / "buildParamsPlots.txt").open("w", encoding="utf-8") as f:
		printGroupPlots(df, algos, "build", f)
		printGroupPlots(df, algos, "indexsize", f)

if __name__ == "__main__":
	main()
