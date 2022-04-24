from __future__ import absolute_import
import h5py as hdf
import os
import traceback

def get_result_filename(
	dataset: str = None, count: int = None, definition=None, efSearch: int = None, batch_mode=False
):
	d = ['results']
	if dataset is not None:
		d.append(dataset)
	if count is not None:
		d.append(str(count))
	if definition:
		d.append(definition.algorithm + ('-batch' if batch_mode else ''))
		d.append(definition.getFilename(efSearch))
	return os.path.join(*d)

def store_results(dataset, count, definition, efSearch, attrs, results, batch):
	fn = get_result_filename(dataset, count, definition, efSearch, batch)
	head, _ = os.path.split(fn)

	if not os.path.isdir(head):
		os.makedirs(head)

	with hdf.File(fn, 'w') as f:
		for k, v in attrs.items():
			f.attrs[k] = v

		times = f.create_dataset('times', (len(results),), 'f')
		neighbors = f.create_dataset('neighbors', (len(results), count), 'i')
		distances = f.create_dataset('distances', (len(results), count), 'f')

		for i, (time, ds) in enumerate(results):
			times[i] = time
			neighbors[i] = [n for n, _ in ds] + [-1] * (count - len(ds))
			distances[i] = [d for _, d in ds] + [float('inf')] * (count - len(ds))

def load_all_results(dataset=None, count=None, batch_mode=False):
	for root, _, files in os.walk(get_result_filename(dataset, count)):
		for fn in files:
			if os.path.splitext(fn)[-1] != '.hdf5':
				continue
			try:
				f = hdf.File(os.path.join(root, fn), 'r+')
				properties = dict(f.attrs)

				if batch_mode != properties['batch_mode']:
					continue

				yield properties, f
				f.close()
			except:
				print('Was unable to read', fn)
				traceback.print_exc()

def get_unique_algorithms():
	algorithms = set()

	for batch_mode in [False, True]:
		for properties, _ in load_all_results(batch_mode=batch_mode):
			algorithms.add(properties['algo'])

	return algorithms
