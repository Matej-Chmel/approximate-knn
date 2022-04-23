import h5py
import numpy
import os
from urllib.request import urlopen
from urllib.request import urlretrieve

def download(src, dst):
	if not os.path.exists(dst):
		# TODO: should be atomic
		print("downloading %s -> %s..." % (src, dst))
		urlretrieve(src, dst)


def get_dataset_fn(dataset):
	if not os.path.exists("data"):
		os.mkdir("data")
	return os.path.join("data", "%s.hdf5" % dataset)


def get_dataset(which):
	hdf5_fn = get_dataset_fn(which)
	try:
		url = "http://ann-benchmarks.com/%s.hdf5" % which
		download(url, hdf5_fn)
	except:
		print("Cannot download %s" % url)
		if which in DATASETS:
			print("Creating dataset locally")
			DATASETS[which](hdf5_fn)
	hdf5_f = h5py.File(hdf5_fn, "r")

	# here for backward compatibility, to ensure old datasets can still be used with newer versions
	# cast to integer because the json parser (later on) cannot interpret numpy integers
	dimension = int(hdf5_f.attrs["dimension"]) if "dimension" in hdf5_f.attrs else len(hdf5_f["train"][0])

	return hdf5_f, dimension


# Everything below this line is related to creating datasets
# You probably never need to do this at home,
# just rely on the prepared datasets at http://ann-benchmarks.com


def write_output(train, test, fn, distance, point_type="float", count=100):
	from ann_benchmarks.algorithms.bruteforce import BruteForceBLAS
	f = h5py.File(fn, "w")
	f.attrs["type"] = "dense"
	f.attrs["distance"] = distance
	f.attrs["dimension"] = len(train[0])
	f.attrs["point_type"] = point_type
	print("train size: %9d * %4d" % train.shape)
	print("test size:  %9d * %4d" % test.shape)
	f.create_dataset("train", (len(train), len(
		train[0])), dtype=train.dtype)[:] = train
	f.create_dataset("test", (len(test), len(
		test[0])), dtype=test.dtype)[:] = test
	neighbors = f.create_dataset("neighbors", (len(test), count), dtype="i")
	distances = f.create_dataset("distances", (len(test), count), dtype="f")
	bf = BruteForceBLAS(distance, precision=train.dtype)

	bf.fit(train)
	for i, x in enumerate(test):
		if i % 1000 == 0:
			print("%d/%d..." % (i, len(test)))
		res = list(bf.query_with_distances(x, count))
		res.sort(key=lambda t: t[-1])
		neighbors[i] = [j for j, _ in res]
		distances[i] = [d for _, d in res]
	f.close()

def train_test_split(X, test_size=10000, dimension=None):
	import sklearn.model_selection
	if dimension == None:
		dimension = X.shape[1]
	print("Splitting %d*%d into train/test" % (X.shape[0], dimension))
	return sklearn.model_selection.train_test_split(
		X, test_size=test_size, random_state=1)


def glove(out_fn, d):
	import zipfile

	url = "http://nlp.stanford.edu/data/glove.twitter.27B.zip"
	fn = os.path.join("data", "glove.twitter.27B.zip")
	download(url, fn)
	with zipfile.ZipFile(fn) as z:
		print("preparing %s" % out_fn)
		z_fn = "glove.twitter.27B.%dd.txt" % d
		X = []
		for line in z.open(z_fn):
			v = [float(x) for x in line.strip().split()[1:]]
			X.append(numpy.array(v))
		X_train, X_test = train_test_split(X)
		write_output(numpy.array(X_train), numpy.array(
			X_test), out_fn, "angular")


def _load_texmex_vectors(f, n, k):
	import struct

	v = numpy.zeros((n, k))
	for i in range(n):
		f.read(4)  # ignore vec length
		v[i] = struct.unpack("f" * k, f.read(k * 4))

	return v


def _get_irisa_matrix(t, fn):
	import struct
	m = t.getmember(fn)
	f = t.extractfile(m)
	k, = struct.unpack("i", f.read(4))
	n = m.size // (4 + 4 * k)
	f.seek(0)
	return _load_texmex_vectors(f, n, k)


def sift(out_fn):
	import tarfile

	url = "ftp://ftp.irisa.fr/local/texmex/corpus/sift.tar.gz"
	fn = os.path.join("data", "sift.tar.tz")
	download(url, fn)
	with tarfile.open(fn, "r:gz") as t:
		train = _get_irisa_matrix(t, "sift/sift_base.fvecs")
		test = _get_irisa_matrix(t, "sift/sift_query.fvecs")
		write_output(train, test, out_fn, "euclidean")


def gist(out_fn):
	import tarfile

	url = "ftp://ftp.irisa.fr/local/texmex/corpus/gist.tar.gz"
	fn = os.path.join("data", "gist.tar.tz")
	download(url, fn)
	with tarfile.open(fn, "r:gz") as t:
		train = _get_irisa_matrix(t, "gist/gist_base.fvecs")
		test = _get_irisa_matrix(t, "gist/gist_query.fvecs")
		write_output(train, test, out_fn, "euclidean")


def _load_mnist_vectors(fn):
	import gzip
	import struct

	print("parsing vectors in %s..." % fn)
	f = gzip.open(fn)
	type_code_info = {
		0x08: (1, "!B"),
		0x09: (1, "!b"),
		0x0B: (2, "!H"),
		0x0C: (4, "!I"),
		0x0D: (4, "!f"),
		0x0E: (8, "!d")
	}
	magic, type_code, dim_count = struct.unpack("!hBB", f.read(4))
	assert magic == 0
	assert type_code in type_code_info

	dimensions = [struct.unpack("!I", f.read(4))[0]
				  for i in range(dim_count)]

	entry_count = dimensions[0]
	entry_size = numpy.product(dimensions[1:])

	b, format_string = type_code_info[type_code]
	vectors = []
	for i in range(entry_count):
		vectors.append([struct.unpack(format_string, f.read(b))[0]
						for j in range(entry_size)])
	return numpy.array(vectors)


def mnist(out_fn):
	download(
		"http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz", "mnist-train.gz")  # noqa
	download(
		"http://yann.lecun.com/exdb/mnist/t10k-images-idx3-ubyte.gz", "mnist-test.gz")  # noqa
	train = _load_mnist_vectors("mnist-train.gz")
	test = _load_mnist_vectors("mnist-test.gz")
	write_output(train, test, out_fn, "euclidean")


def fashion_mnist(out_fn):
	download("http://fashion-mnist.s3-website.eu-central-1.amazonaws.com/train-images-idx3-ubyte.gz",  # noqa
			 "fashion-mnist-train.gz")
	download("http://fashion-mnist.s3-website.eu-central-1.amazonaws.com/t10k-images-idx3-ubyte.gz",  # noqa
			 "fashion-mnist-test.gz")
	train = _load_mnist_vectors("fashion-mnist-train.gz")
	test = _load_mnist_vectors("fashion-mnist-test.gz")
	write_output(train, test, out_fn, "euclidean")

# Creates a "deep image descriptor" dataset using the "deep10M.fvecs" sample
# from http://sites.skoltech.ru/compvision/noimi/. The download logic is adapted
# from the script https://github.com/arbabenko/GNOIMI/blob/master/downloadDeep1B.py.
def deep_image(out_fn):
	yadisk_key = "https://yadi.sk/d/11eDCm7Dsn9GA"
	response = urlopen("https://cloud-api.yandex.net/v1/disk/public/resources/download?public_key=" \
		+ yadisk_key + "&path=/deep10M.fvecs")
	response_body = response.read().decode("utf-8")

	dataset_url = response_body.split(",")[0][9:-1]
	filename = os.path.join("data", "deep-image.fvecs")
	download(dataset_url, filename)

	# In the fvecs file format, each vector is stored by first writing its
	# length as an integer, then writing its components as floats.
	fv = numpy.fromfile(filename, dtype=numpy.float32)
	dim = fv.view(numpy.int32)[0]
	fv = fv.reshape(-1, dim + 1)[:, 1:]

	X_train, X_test = train_test_split(fv)
	write_output(X_train, X_test, out_fn, "angular")

def transform_bag_of_words(filename, n_dimensions, out_fn):
	import gzip
	from scipy.sparse import lil_matrix
	from sklearn.feature_extraction.text import TfidfTransformer
	from sklearn import random_projection
	with gzip.open(filename, "rb") as f:
		file_content = f.readlines()
		entries = int(file_content[0])
		words = int(file_content[1])
		file_content = file_content[3:]  # strip first three entries
		print("building matrix...")
		A = lil_matrix((entries, words))
		for e in file_content:
			doc, word, cnt = [int(v) for v in e.strip().split()]
			A[doc - 1, word - 1] = cnt
		print("normalizing matrix entries with tfidf...")
		B = TfidfTransformer().fit_transform(A)
		print("reducing dimensionality...")
		C = random_projection.GaussianRandomProjection(
			n_components=n_dimensions).fit_transform(B)
		X_train, X_test = train_test_split(C)
		write_output(numpy.array(X_train), numpy.array(
			X_test), out_fn, "angular")


def nytimes(out_fn, n_dimensions):
	fn = "nytimes_%s.txt.gz" % n_dimensions
	download("https://archive.ics.uci.edu/ml/machine-learning-databases/bag-of-words/docword.nytimes.txt.gz", fn)  # noqa
	transform_bag_of_words(fn, n_dimensions, out_fn)


def random_float(out_fn, n_dims, n_samples, centers, distance):
	import sklearn.datasets

	X, _ = sklearn.datasets.make_blobs(
		n_samples=n_samples, n_features=n_dims,
		centers=centers, random_state=1)
	X_train, X_test = train_test_split(X, test_size=0.1)
	write_output(X_train, X_test, out_fn, distance)


def lastfm(out_fn, n_dimensions, test_size=50000):
	# This tests out ANN methods for retrieval on simple matrix factorization
	# based recommendation algorithms. The idea being that the query/test
	# vectors are user factors and the train set are item factors from
	# the matrix factorization model.

	# Since the predictor is a dot product, we transform the factors first
	# as described in this
	# paper: https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/XboxInnerProduct.pdf  # noqa
	# This hopefully replicates the experiments done in this post:
	# http://www.benfrederickson.com/approximate-nearest-neighbours-for-recommender-systems/  # noqa

	# The dataset is from "Last.fm Dataset - 360K users":
	# http://www.dtic.upf.edu/~ocelma/MusicRecommendationDataset/lastfm-360K.html  # noqa

	# This requires the implicit package to generate the factors
	# (on my desktop/gpu this only takes 4-5 seconds to train - but
	# could take 1-2 minutes on a laptop)
	from implicit.datasets.lastfm import get_lastfm
	from implicit.approximate_als import augment_inner_product_matrix
	import implicit

	# train an als model on the lastfm data
	_, _, play_counts = get_lastfm()
	model = implicit.als.AlternatingLeastSquares(factors=n_dimensions)
	model.fit(implicit.nearest_neighbours.bm25_weight(
		play_counts, K1=100, B=0.8))

	# transform item factors so that each one has the same norm,
	# and transform the user factors such by appending a 0 column
	_, item_factors = augment_inner_product_matrix(model.item_factors)
	user_factors = numpy.append(model.user_factors,
								numpy.zeros((model.user_factors.shape[0], 1)),
								axis=1)

	# only query the first 50k users (speeds things up signficantly
	# without changing results)
	user_factors = user_factors[:test_size]

	# after that transformation a cosine lookup will return the same results
	# as the inner product on the untransformed data
	write_output(item_factors, user_factors, out_fn, "angular")


DATASETS = {
	"deep-image-96-angular": deep_image,
	"fashion-mnist-784-euclidean": fashion_mnist,
	"gist-960-euclidean": gist,
	"glove-25-angular": lambda out_fn: glove(out_fn, 25),
	"glove-50-angular": lambda out_fn: glove(out_fn, 50),
	"glove-100-angular": lambda out_fn: glove(out_fn, 100),
	"glove-200-angular": lambda out_fn: glove(out_fn, 200),
	"lastfm-64-dot": lambda out_fn: lastfm(out_fn, 64),
	"mnist-784-euclidean": mnist,
	"nytimes-16-angular": lambda out_fn: nytimes(out_fn, 16),
	"nytimes-256-angular": lambda out_fn: nytimes(out_fn, 256),
	"random-s-100-angular": lambda out_fn: random_float(out_fn, 100, 100000, 1000, "angular"),
	"random-s-100-euclidean": lambda out_fn: random_float(out_fn, 100, 100000, 1000, "euclidean"),
	"random-xs-20-angular": lambda out_fn: random_float(out_fn, 20, 10000, 100, "angular"),
	"random-xs-20-euclidean": lambda out_fn: random_float(out_fn, 20, 10000, 100, "euclidean"),
	"sift-128-euclidean": sift
}
