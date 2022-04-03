from __future__ import absolute_import
from ann_benchmarks.algorithms.base import BaseANN
import chm_hnsw as h
import numpy as np


class ChmHnsw(BaseANN):
	def __init__(self, indexCls, metric: str, params: dict, simdType: h.SIMDType):
		self.indexCls = indexCls
		self.params = params
		self.simdType = simdType
		self.space = {'angular': h.Space.ANGULAR, 'euclidean': h.Space.EUCLIDEAN}[metric]

	def fit(self, X):
		self.index = self.indexCls(
			dim=len(X[0]), maxCount=len(X), efConstruction=self.params["efConstruction"],
			mMax=self.params["mMax"], seed=100, SIMD=self.simdType, space=self.space
		)
		self.name = str(self.index)
		self.index.push(np.asarray(X))

	def freeIndex(self):
		del self.index

	def query(self, v, n: int):
		return self.index.queryBatch(np.expand_dims(v, axis=0), n)[0][0]

	def set_query_arguments(self, ef: int):
		self.index.setEfSearch(ef)

def ChmHnswAVX(metric: str, method_param: dict):
	return ChmHnsw(h.HeuristicIndex, metric, method_param, h.SIMDType.AVX)

def ChmHnswHeuristic(metric: str, method_param: dict):
	return ChmHnsw(h.HeuristicIndex, metric, method_param, h.SIMDType.BEST)

def ChmHnswNaive(metric: str, method_param: dict):
	return ChmHnsw(h.NaiveIndex, metric, method_param, h.SIMDType.BEST)

def ChmHnswNoSIMD(metric: str, method_param: dict):
	return ChmHnsw(h.HeuristicIndex, metric, method_param, h.SIMDType.NONE)

def ChmHnswPrefetching(metric: str, method_param: dict):
	return ChmHnsw(h.PrefetchingIndex, metric, method_param, h.SIMDType.BEST)

def ChmHnswSSE(metric: str, method_param: dict):
	return ChmHnsw(h.HeuristicIndex, metric, method_param, h.SIMDType.SSE)
