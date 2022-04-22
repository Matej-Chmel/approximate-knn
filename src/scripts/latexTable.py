from argparse import ArgumentParser
from chmDataset import runner
from chmDataset.export import getExportedData
from dataclasses import dataclass, field
from functools import cached_property
import parse
import pandas as pd
from pathlib import Path

N = "\n"

class Algorithm:
	def __init__(self, s: pd.Series):
		params: str = s["parameters"]

		if params.startswith("chm"):
			parsed = parse.parse("chm(e={},m={},{}", params)
		elif params.startswith("hnswlib"):
			parsed = parse.parse("hnswlib(e={},m={}){}", params)
		else:
			raise ValueError(f"Unknown parameters: {params}")

		self.efConstruction = int(parsed[0])
		self.mMax = int(parsed[1])
		self.name: str = s["algorithm"]
		self.buildTime = int(s["build"])
		self.indexSize = int(s["indexsize"]) // 1000

@dataclass
class Args:
	algoNames: list[str]
	calcPercent: bool
	dataset: str
	label: str
	legend: list[str]
	output: Path
	recompute: bool

	def __post_init__(self):
		if len(self.algoNames) != len(self.legend):
			raise ValueError("Algorithms names and legend must have the same length.")

@dataclass
class Row:
	efConstruction: int
	mMax: int
	algoToBuildTime: dict[str, float] = field(default_factory=dict)
	algoToIndexSize: dict[str, float] = field(default_factory=dict)

	def getString(self, algoNames: list[str], calcPercent: bool):
		if calcPercent:
			time0 = self.algoToBuildTime[algoNames[0]]
			time1 = self.algoToBuildTime[algoNames[1]]
			buildTimes = [
				time0, getPercentDiff(time0, time1), time1,
				*[self.algoToBuildTime[algoNames[i]] for i in range(2, len(algoNames))]
			]
		else:
			buildTimes = [self.algoToBuildTime[algoName] for algoName in algoNames]
		return " & ".join(map(str, [
			self.efConstruction, self.mMax,
			*buildTimes,
			*[self.algoToIndexSize[algoName] for algoName in algoNames]
		])) + r"\\"

	@cached_property
	def paramHash(self):
		return self.efConstruction + self.mMax

@dataclass
class Table:
	algoNames: list[str]
	calcPercent: bool = False
	caption: str = ""
	label: str = ""
	legend: list[str] = None
	rows: list[Row] = field(default_factory=list)

	def add(self, algo: Algorithm):
		try:
			row = next(filter(
				lambda row: row.efConstruction == algo.efConstruction and row.mMax == algo.mMax,
				self.rows
			))
		except StopIteration:
			row = Row(algo.efConstruction, algo.mMax)
			self.rows.append(row)

		row.algoToBuildTime[algo.name] = algo.buildTime
		row.algoToIndexSize[algo.name] = algo.indexSize

	def getColumns(self):
		return "r" * (7 if self.calcPercent else 6)

	def getContent(self):
		return N.join(map(
			lambda row: row.getString(self.algoNames, self.calcPercent),
			sorted(self.rows, key=lambda row: row.paramHash)
		))

	def getHeaders(self):
		algoLen = len(self.algoNames)
		buildLen = algoLen + 1 if self.calcPercent else algoLen
		legend = self.algoNames if self.legend is None else self.legend
		names = " & ".join(legend)
		buildNames = f"{legend[0]} & Rozdíl (\\%) & {' & '.join(legend[1:])}" if self.calcPercent else names
		return (
			r"\multicolumn{2}{c}{Konfigurace} & "
			f"\multicolumn{{{buildLen}}}{{c}}{{Čas stavby (s)}} & "
			f"\multicolumn{{{algoLen}}}{{c}}{{Velikost indexu (1000 kB)}}\\\\{N}"
			"$ef_{construction}$ & $M_{max}$ & "
			f"{buildNames} & {names}"
		)

	def getLatex(self, template: str):
		return template.replace("@CAPTION@", self.caption
		).replace("@COLUMNS@", self.getColumns()
		).replace("@CONTENT@", self.getContent()
		).replace("@HEADERS@", self.getHeaders()
		).replace("@LABEL@", self.label)

def getArgs():
	p = ArgumentParser(
		"LATEX_TABLE", description="Generates a LaTeX table of build times and index sizes."
	)
	p.add_argument("-a", "--algorithms", help="List of algorithms.", nargs="+", required=True)
	p.add_argument("-d", "--dataset", help="Name of dataset.", required=True)
	p.add_argument("-la", "--label", help="Label of the table.", required=True)
	p.add_argument("-le", "--legend", help="Legend of the table.", nargs="+", required=True)
	p.add_argument("-o", "--output", help="Path to output file.", required=True)
	p.add_argument(
		"-p", "--percent", action="store_true", help="Calculate percent difference in build times."
	)
	p.add_argument("-r", "--recompute", action="store_true", help="Recompute performance metrics.")
	args = p.parse_args()
	return Args(
		args.algorithms, args.percent, args.dataset, args.label,
		args.legend, Path(args.output), args.recompute
	)

def getPercentDiff(orig: float, new: float):
	return round((new - orig) / orig * 100, 2)

def getTable(df: pd.DataFrame, algoNames: list[str], dataset: str):
	table = Table(algoNames)
	df = df[df["dataset"] == dataset]

	for algo in algoNames:
		algoDf = df[df["algorithm"] == algo].drop_duplicates(["parameters"])

		for _, s in algoDf.iterrows():
			table.add(Algorithm(s))

	return table

def writeTable(a: Args):
	a.output.parent.mkdir(exist_ok=True, parents=True)

	with a.output.open("w", encoding="utf-8") as f:
		table = getTable(getExportedData(a.recompute), a.algoNames, a.dataset)
		table.calcPercent = a.calcPercent
		table.caption = "Tabulka časů stavby a velikostí indexu."
		table.label = a.label
		table.legend = a.legend
		f.write(table.getLatex(
			(Path(__file__).parent / "templates" / "latexTable.txt").read_text(encoding="utf-8")
		))

def main():
	writeTable(getArgs())

if __name__ == "__main__":
	runner.run(main)
