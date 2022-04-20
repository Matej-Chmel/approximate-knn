from argparse import ArgumentParser
from dataclasses import astuple, dataclass, field
import pandas as pd
import parse
from pathlib import Path
import subprocess
import sys
from typing import Callable

BACK_SLASH = "\\"
N = "\n"
LONG_TO_SHORT_DATASET_NAMES = {
	"glove-50-angular": "GloVe-50",
	"lastfm-64-dot": "Last.fm",
	"nytimes-256-angular": "NYTimes",
	"mnist-784-euclidean": "MNIST",
	"sift-128-euclidean": "SIFT"
}
TAB = "\t"

@dataclass
class Args:
	algorithms: list[str]
	datasets: list[str]
	legend: list[str]
	outputPath: Path
	plots: list[str]
	recompute: bool
	srcDir: Path

	def __post_init__(self):
		if not self.legend:
			self.legend = None

@dataclass
class PlotValue:
	x: float
	y: float

	def getTupleStr(self):
		return str(astuple(self))

@dataclass
class Algorithm:
	name: str
	plotValues: list[PlotValue] = field(default_factory=list)

@dataclass
class Dataset:
	name: str
	algos: list[Algorithm] = field(default_factory=list)

	def __post_init__(self):
		try:
			self.name = LONG_TO_SHORT_DATASET_NAMES[self.name]
		except KeyError:
			print(f"Dataset {self.name} doesn't have a short name assigned to it.")

	def getGroupPlot(self):
		res = f"{BACK_SLASH}nextgroupplot[title={self.name}]{N}"

		for algo in self.algos:
			res += f"{BACK_SLASH}addplot coordinates {{{N}% {algo.name}{N}"

			for val in sorted(algo.plotValues, key=lambda v: v.x):
				res += f"{TAB}{val.getTupleStr()}{N}"

			res += f"}};{N}"

		return res

@dataclass
class Plot:
	algos: list[str]
	bestDirection: str
	caption: str
	datasets: list[str]
	df: pd.DataFrame
	plotLabel: str
	rowToX: Callable[[pd.Series], float]
	rowToY: Callable[[pd.Series], float]
	xLabel: str
	yLabel: str
	dropDuplicateParameters: bool = False
	legend: list[str] = None
	shortCaption: str = None
	yModeLog: bool = False

	def getAlgorithm(self, algoName: str, df: pd.DataFrame):
		algoDf = df[df["algorithm"] == algoName]

		if self.dropDuplicateParameters:
			algoDf = algoDf.drop_duplicates(["parameters"])

		return Algorithm(
			algoName, [PlotValue(self.rowToX(row), self.rowToY(row)) for _, row in algoDf.iterrows()]
		)

	def getDataset(self, datasetName: str):
		df = self.df[self.df["dataset"] == datasetName]
		return Dataset(datasetName, [self.getAlgorithm(algoName, df) for algoName in self.algos])

	def getPlotPart(self, datasetNames: list[str], template: str):
		return template.replace(
			"@GROUP_SIZE@", "2 by 2" if len(datasetNames) == 4 else f"{len(datasetNames)} by 1"
		).replace("@YMODE@", f"{N}ymode = log," if self.yModeLog else ""
		).replace("@XLABEL@", self.xLabel
		).replace("@YLABEL@", self.yLabel
		).replace("@GROUP_PLOTS@", "\n\n".join([
			self.getDataset(name).getGroupPlot()
			for name in datasetNames
		])).replace("@LEGEND@", ",".join(self.legend if self.legend is not None else self.algos)
		).replace("@SHORT_CAPTION@", f"[{self.shortCaption}]" if self.shortCaption is not None else ""
		).replace("@LONG_CAPTION@", self.caption
		).replace("@BEST_DIRECTION@", self.bestDirection
		).replace("@LABEL@", self.plotLabel)

	def getPlotStr(self, template: str):
		res = ""

		for g in getListOfChunks(self.datasets, 4):
			res += self.getPlotPart(g, template)

		return res

def getArgs(srcDir: Path):
	plotChoices = ["build", "recall", "size"]
	p = ArgumentParser("LATEX_GROUP_PLOTS", description="Builds LaTeX group plots.")
	p.add_argument("-a", "--algorithms", help="List of algorithm names.", nargs="+", required=True)
	p.add_argument("-d", "--datasets", help="List of dataset names.", nargs="+", required=True)
	p.add_argument("-l", "--legend", help="List of legend labels.", nargs="*")
	p.add_argument("-o", "--output", default=srcDir / "groupPlots.tex", help="Path to output file.")
	p.add_argument(
		"-p", "--plots", choices=plotChoices, default=plotChoices,
		help="Type of plots to build.", nargs="*"
	)
	p.add_argument("-r", "--recompute", action="store_true", help="Recompute exported data.")
	args = p.parse_args()
	return Args(
		args.algorithms, args.datasets, args.legend, Path(args.output),
		args.plots, args.recompute, srcDir
	)

def getBuildSeconds(s: pd.Series):
	return s["build"]

def getHashBuildPlot(a: Args, df: pd.DataFrame):
	caption = "Závislost času stavby na parametrech stavby."
	return Plot(
		algos=a.algorithms, bestDirection="Lepší výsledky směrem k dolnímu okraji grafu.",
		caption=caption, datasets=a.datasets, df=df, plotLabel="HashBuild",
		rowToX=getParamsHash, rowToY=getBuildSeconds, xLabel="$ef_{construction} + M_{max}$",
		yLabel="Čas stavby (s)", dropDuplicateParameters=True, legend=a.legend,
		shortCaption=caption, yModeLog=True
	)

def getHashSizePlot(a: Args, df: pd.DataFrame):
	caption = "Závislost velikosti indexu na parametrech stavby."
	return Plot(
		algos=a.algorithms, bestDirection="Lepší výsledky směrem k dolnímu okraji grafu.",
		caption=caption, datasets=a.datasets, df=df, plotLabel="HashSize",
		rowToX=getParamsHash, rowToY=getIndexSize, xLabel="$ef_{construction} + M_{max}$",
		yLabel="Velikost indexu (kB)", dropDuplicateParameters=True, legend=a.legend,
		shortCaption=caption
	)

def getIndexSize(s: pd.Series):
	return s["indexsize"]

def getListOfChunks(l: list, chunkLen: int):
	return [l[i * chunkLen:(i + 1) * chunkLen] for i in range((len(l) + chunkLen - 1) // chunkLen)]

def getParamsHash(s: pd.Series):
	params = s["parameters"]
	parseRes = (
		parse.parse("chm(e={},m={},d={},n={},p={})", params) if params.startswith("chm")
		else parse.parse("hnswlib(e={},m={})", params)
	)
	return int(parseRes[0]) + int(parseRes[1])

def getQueriesPerSecond(s: pd.Series):
	return s["qps"]

def getRecall(s: pd.Series):
	return s["k-nn"]

def getRecallQueriesPlot(a: Args, df: pd.DataFrame):
	caption = "Závislost počtu dotazů za sekundu na přesnosti."
	return Plot(
		algos=a.algorithms, bestDirection="Lepší výsledky směrem k pravému hornímu rohu grafu.",
		caption=caption, datasets=a.datasets, df=df, plotLabel="RecallQueries",
		rowToX=getRecall, rowToY=getQueriesPerSecond, xLabel="Recall",
		yLabel=r"Počet dotazů za sekundu $(\frac{1}{s})$", legend=a.legend, shortCaption=caption,
		yModeLog=True
	)

def writePlots(a: Args):
	inputPath = a.srcDir / "figures" / "data.csv"
	inputPath.parent.mkdir(exist_ok=True, parents=True)

	if a.recompute or not inputPath.exists():
		subprocess.call([
			sys.executable, "data_export.py", "-o", inputPath.absolute()] + (["-r"] if a.recompute else []),
			cwd=a.srcDir / "benchmarks"
		)

	with a.outputPath.open("w", encoding="utf-8") as f:
		df = pd.read_csv(inputPath, sep=",")
		template = (a.srcDir / "scripts" / "templates" / "latexGroupPlots.txt").read_text(encoding="utf-8")

		for plotType in a.plots:
			f.write({
				"build": getHashBuildPlot,
				"recall": getRecallQueriesPlot,
				"size": getHashSizePlot
			}[plotType](a, df).getPlotStr(template))

def main():
	writePlots(getArgs(Path(__file__).parents[1]))

if __name__ == "__main__":
	main()
