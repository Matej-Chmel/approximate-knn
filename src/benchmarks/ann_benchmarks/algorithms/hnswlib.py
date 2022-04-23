from __future__ import absolute_import
import hnswlib
import numpy as np
from ann_benchmarks.algorithms.base import BaseANN


class HnswLib(BaseANN):
	def __init__(self, efConstruction: int, M: int, metric: str):
		self.efConstruction = efConstruction
		self.M = M
		self.metric = {'angular': 'cosine', 'euclidean': 'l2'}[metric]
		self.name = f"hnswlib(e={self.efConstruction},m={self.M})"

	def __str__(self):
		return f"{self.name}[ef={self.ef}]"

	def done(self):
		del self.p

	def fit(self, X):
		self.p = hnswlib.Index(space=self.metric, dim=len(X[0]))
		self.p.init_index(max_elements=len(X), ef_construction=self.efConstruction, M=self.M)
		data_labels = np.arange(len(X))
		self.p.set_num_threads(1)
		self.p.add_items(np.asarray(X), data_labels)

	def query(self, v, n):
		return self.p.knn_query(np.expand_dims(v, axis=0), k=n)[0][0]

	def set_query_arguments(self, ef):
		self.ef = ef
		self.p.set_ef(ef)
