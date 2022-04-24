from __future__ import absolute_import
from scipy.spatial.distance import pdist as scipy_pdist
import numpy as np

def pdist(a, b, metric):
	return scipy_pdist([a, b], metric=metric)[0]

metrics = {
	'angular': {
		'distance': lambda a, b: pdist(a, b, "cosine"),
		'distance_valid': lambda _: True
	},
	'euclidean': {
		'distance': lambda a, b: pdist(a, b, "euclidean"),
		'distance_valid': lambda _: True
	}
}

def sparse_to_lists(data, lengths):
	X = []
	index = 0
	for l in lengths:
		X.append(data[index:index+l])
		index += l

	return X

def dataset_transform(dataset):
	if dataset.attrs.get('type', 'dense') != 'sparse':
		return np.array(dataset['train']), np.array(dataset['test'])

	# we store the dataset as a list of integers, accompanied by a list of lengths in hdf5
	# so we transform it back to the format expected by the algorithms here (array of array of ints)
	return sparse_to_lists(dataset['train'], dataset['size_train']), sparse_to_lists(dataset['test'], dataset['size_test'])
