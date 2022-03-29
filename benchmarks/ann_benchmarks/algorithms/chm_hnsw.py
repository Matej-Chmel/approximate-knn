from __future__ import absolute_import
from ann_benchmarks.algorithms.base import BaseANN
from chm_hnsw import Index, Space
import numpy as np


class ChmHnswHeuristic(BaseANN):
	def __init__(self, metric: str, method_param: dict):
		self.params = method_param
		self.space = {'angular': Space.ANGULAR, 'euclidean': Space.EUCLIDEAN}[metric]

	def fit(self, X):
		self.index = Index(
			len(X[0]), self.params["efConstruction"], len(X),
			self.params["mMax"], 100, self.space, True
		)
		self.name = f"{self.index}[heuristic]"
		self.index.push(np.asarray(X))

	def freeIndex(self):
		del self.index

	def query(self, v, n: int):
		return self.index.query(np.expand_dims(v, axis=0), n)[0][0]

	def set_query_arguments(self, ef: int):
		self.index.setEfSearch(ef)


class ChmHnswNaive(BaseANN):
	def __init__(self, metric: str, method_param: dict):
		self.params = method_param
		self.space = {'angular': Space.ANGULAR, 'euclidean': Space.EUCLIDEAN}[metric]

	def fit(self, X):
		self.index = Index(
			len(X[0]), self.params["efConstruction"], len(X),
			self.params["mMax"], 100, self.space, False
		)
		self.name = f"{self.index}[naive]"
		self.index.push(np.asarray(X))

	def freeIndex(self):
		del self.index

	def query(self, v, n: int):
		return self.index.query(np.expand_dims(v, axis=0), n)[0][0]

	def set_query_arguments(self, ef: int):
		self.index.setEfSearch(ef)
