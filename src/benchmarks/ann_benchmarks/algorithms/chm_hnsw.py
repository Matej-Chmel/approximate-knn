from __future__ import absolute_import
from ann_benchmarks.algorithms.base import BaseANN
import chm_hnsw as h
import numpy as np


class ChmHnsw(BaseANN):
	def __init__(self, indexCls, efConstruction: int, metric: str, mMax: int, simdType: h.SIMDType):
		self.efConstruction = efConstruction
		self.indexCls = indexCls
		self.mMax = mMax
		self.simdType = simdType
		self.space = {'angular': h.Space.ANGULAR, 'euclidean': h.Space.EUCLIDEAN}[metric]

	def __str__(self):
		return f"{self.index}[ef={self.efSearch}]"

	def done(self):
		del self.index

	def fit(self, X):
		self.index = self.indexCls(
			dim=len(X[0]), maxCount=len(X), efConstruction=self.efConstruction,
			mMax=self.mMax, seed=100, SIMD=self.simdType, space=self.space
		)
		self.index.push(np.asarray(X))

	def query(self, v, n: int):
		return self.index.queryBatch(np.expand_dims(v, axis=0), n)[0][0]

	def set_query_arguments(self, efSearch: int):
		self.efSearch = efSearch
		self.index.setEfSearch(efSearch)

def ChmHnswAVX(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.HeuristicIndex, efConstruction, metric, mMax, h.SIMDType.AVX)

def ChmHnswHeuristic(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.HeuristicIndex, efConstruction, metric, mMax, h.SIMDType.BEST)

def ChmHnswNaive(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.NaiveIndex, efConstruction, metric, mMax, h.SIMDType.BEST)

def ChmHnswNoBitArray(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.NoBitArrayIndex, efConstruction, metric, mMax, h.SIMDType.BEST)

def ChmHnswNoSIMD(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.HeuristicIndex, efConstruction, metric, mMax, h.SIMDType.NONE)

def ChmHnswPrefetching(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.PrefetchingIndex, efConstruction, metric, mMax, h.SIMDType.BEST)

def ChmHnswSSE(efConstruction: int, mMax: int, metric: str):
	return ChmHnsw(h.HeuristicIndex, efConstruction, metric, mMax, h.SIMDType.SSE)
