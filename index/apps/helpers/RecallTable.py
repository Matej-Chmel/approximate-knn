from chm_hnsw import getRecall, Index, Space
from dataclasses import dataclass
from .Dataset import Dataset
import pandas
from pathlib import Path
import time

EF_SEARCH_WIDTH = 8
ELAPSED_PRETTY_WIDTH = 29
ELAPSED_WIDTH = 29
N = "\n"
RECALL_WIDTH = 12

def getPrettyElapsedStr(elapsedNS: int):
	return str(pandas.Timedelta(nanoseconds=elapsedNS))

@dataclass
class QueryBenchmark:
	efSearch: int
	elapsedNS: int = None
	recall: float = None

	def getPrettyElapsedStr(self):
		return getPrettyElapsedStr(self.elapsedNS)

class RecallTable:
	def __init__(self, dataset: Dataset, efSearchValues: list[int]):
		self.benchmarks: list[QueryBenchmark] = None
		self.buildElapsedNS: int = None
		self.dataset = dataset
		self.efSearchValues = efSearchValues
		self.index: Index = None
		self.space = Space.ANGULAR if self.dataset.angular else Space.EUCLIDEAN

	def __str__(self):
		return "\n".join([
			str(self.dataset),
			str(self.index),
			f"Build time: [{getPrettyElapsedStr(self.buildElapsedNS)}], {self.buildElapsedNS} ns{N}",
			f"{'EfSearch':>{EF_SEARCH_WIDTH}}{'Recall':>{RECALL_WIDTH}}{'Elapsed (pretty)':>{ELAPSED_PRETTY_WIDTH}}{'Elapsed (ns)':>{ELAPSED_WIDTH}}",
			*[
				f"{b.efSearch:>{EF_SEARCH_WIDTH}}{b.recall:>{RECALL_WIDTH}.3f}{b.getPrettyElapsedStr():>{ELAPSED_PRETTY_WIDTH}}{b.elapsedNS:>{ELAPSED_WIDTH}}"
				for b in self.benchmarks
			]
		])

	@classmethod
	def fromHDF(cls, datasetPath: Path, efSearchValues: list[int]):
		return cls(Dataset.fromHDF(datasetPath), efSearchValues)

	def run(self, efConstruction: int, mMax: int, seed: int):
		self.benchmarks = []
		print("Building index.")

		start = time.perf_counter_ns()
		self.index = Index(self.dataset.dim, efConstruction, self.dataset.trainCount, mMax, seed, self.space)
		self.index.push(self.dataset.train)
		end = time.perf_counter_ns()

		self.buildElapsedNS = end - start
		print(f"Index built in {getPrettyElapsedStr(self.buildElapsedNS)}.", end = "\n\n")

		for efSearch in self.efSearchValues:
			benchmark = QueryBenchmark(efSearch)
			self.benchmarks.append(benchmark)
			print(f"Querying with efSearch = {efSearch}.")

			start = time.perf_counter_ns()
			self.index.setEfSearch(efSearch)
			knnResults = self.index.query(self.dataset.test, self.dataset.k)
			end = time.perf_counter_ns()
			benchmark.elapsedNS = end - start
			print(f"Completed in {benchmark.getPrettyElapsedStr()}.")

			print("Computing recall.")
			start = time.perf_counter_ns()
			benchmark.recall = getRecall(self.dataset.neighbors, knnResults[0])
			end = time.perf_counter_ns()
			recallElapsedNS = end - start
			print(f"Recall computed in {getPrettyElapsedStr(recallElapsedNS)}.", end="\n\n")
