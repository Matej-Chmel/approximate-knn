from __future__ import absolute_import
import hnswlib
import numpy as np
from ann_benchmarks.algorithms.base import BaseANN


class HnswLib(BaseANN):
	def __init__(self, metric, method_param):
		self.metric = {'angular': 'cosine', 'euclidean': 'l2'}[metric]
		self.method_param = method_param
		self.name = f"hnswlib(e={self.method_param['efConstruction']},m={self.method_param['M']})"

	def fit(self, X):
		self.p = hnswlib.Index(space=self.metric, dim=len(X[0]))
		self.p.init_index(
			max_elements=len(X), ef_construction=self.method_param["efConstruction"],
			M=self.method_param["M"]
		)
		data_labels = np.arange(len(X))
		self.p.set_num_threads(1)
		self.p.add_items(np.asarray(X), data_labels)

	def set_query_arguments(self, ef):
		self.p.set_ef(ef)

	def query(self, v, n):
		return self.p.knn_query(np.expand_dims(v, axis=0), k=n)[0][0]

	def freeIndex(self):
		del self.p
