import chm_hnsw as h
from dataclasses import dataclass
from .Dataset import Dataset
import json
import pandas
from pathlib import Path
from .runner import AppError
import time

EF_SEARCH_WIDTH = 8
ELAPSED_PRETTY_WIDTH = 29
ELAPSED_WIDTH = 29
N = "\n"
RECALL_WIDTH = 12

@dataclass
class RecallTableConfig:
	dataset: str
	efConstruction: int
	efSearch: list[int]
	indexTemplate: h.IndexTemplate
	mMax: int
	seed: int
	simdType: h.SIMDType

	@classmethod
	def fromDict(cls, d: dict):
		if not isinstance(d, dict):
			raise AppError(f"Index configuration must be a JSON object. Was:{N}{d}")
		try:
			return RecallTableConfig(
				d["dataset"], d["efConstruction"], d["efSearch"],
				h.getIndexTemplate(d["template"]), d["mMax"], d["seed"],
				h.SIMDType.NONE
				if "SIMD" not in d or d["SIMD"] is None
				else h.getSIMDType(d["SIMD"])
			)
		except KeyError as e:
			raise AppError(f'Missing key {e.args[0]} in object:{N}{d}')

	def getIndexCls(self):
		return {
			h.IndexTemplate.HEURISTIC: h.HeuristicIndex,
			h.IndexTemplate.NAIVE: h.NaiveIndex,
			h.IndexTemplate.NO_BIT_ARRAY: h.NoBitArrayIndex,
			h.IndexTemplate.PREFETCHING: h.PrefetchingIndex
		}[self.indexTemplate]

	@classmethod
	def listFromJSON(cls, p: Path):
		with p.open("r", encoding="utf-8") as f:
			arr = json.load(f)

			if not isinstance(arr, list):
				raise AppError(f'Root element of "recallTable.json" must be a JSON array. Was:{N}{arr}')
			return [cls.fromDict(e) for e in arr]

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
	def __init__(self, cfg: RecallTableConfig, dataset: Dataset):
		self.benchmarks: list[QueryBenchmark] = None
		self.buildElapsedNS: int = None
		self.cfg = cfg
		self.dataset = dataset
		self.indexStr: str = None
		self.space = h.Space.ANGULAR if self.dataset.angular else h.Space.EUCLIDEAN

	def __str__(self):
		if self.indexStr is None:
			raise AppError("Recall table not yet computed.")
		return "\n".join([
			str(self.dataset),
			self.indexStr,
			f"Build time: [{getPrettyElapsedStr(self.buildElapsedNS)}], {self.buildElapsedNS} ns{N}",
			f"{'EfSearch':>{EF_SEARCH_WIDTH}}{'Recall':>{RECALL_WIDTH}}{'Elapsed (pretty)':>{ELAPSED_PRETTY_WIDTH}}{'Elapsed (ns)':>{ELAPSED_WIDTH}}",
			*[
				f"{b.efSearch:>{EF_SEARCH_WIDTH}}{b.recall:>{RECALL_WIDTH}.3f}{b.getPrettyElapsedStr():>{ELAPSED_PRETTY_WIDTH}}{b.elapsedNS:>{ELAPSED_WIDTH}}"
				for b in self.benchmarks
			]
		])

	@classmethod
	def fromHDF(cls, cfg: RecallTableConfig, datasetPath: Path):
		return cls(cfg, Dataset.fromHDF(datasetPath))

	def run(self):
		self.benchmarks = []
		print("Building index.")

		start = time.perf_counter_ns()
		index = self.cfg.getIndexCls()(
			self.dataset.dim, self.dataset.trainCount,
			self.cfg.efConstruction, self.cfg.mMax, self.cfg.seed, self.cfg.simdType, self.space
		)
		index.push(self.dataset.train)
		end = time.perf_counter_ns()

		self.indexStr = str(index)
		self.buildElapsedNS = end - start
		print(f"Index built in {getPrettyElapsedStr(self.buildElapsedNS)}.", end = "\n\n")

		for efSearch in self.cfg.efSearch:
			benchmark = QueryBenchmark(efSearch)
			self.benchmarks.append(benchmark)
			print(f"Querying with efSearch = {efSearch}.")

			start = time.perf_counter_ns()
			index.setEfSearch(efSearch)
			knnResults = index.queryBatch(self.dataset.test, self.dataset.k)
			end = time.perf_counter_ns()
			benchmark.elapsedNS = end - start
			print(f"Completed in {benchmark.getPrettyElapsedStr()}.")

			print("Computing recall.")
			start = time.perf_counter_ns()
			benchmark.recall = h.getRecall(self.dataset.neighbors, knnResults[0])
			end = time.perf_counter_ns()
			recallElapsedNS = end - start
			print(f"Recall computed in {getPrettyElapsedStr(recallElapsedNS)}.", end="\n\n")
