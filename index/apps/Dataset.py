from dataclasses import dataclass
from io import BufferedWriter
import h5py as hdf
from pathlib import Path
import numpy as np
from sklearn.neighbors import NearestNeighbors
import struct

def generateData(count: int, dim: int, seed: int):
	np.random.seed(seed)
	return np.random.rand(count, dim).astype(np.float32)

def getNeighbors(angular: bool, k: int, test: np.ndarray, train: np.ndarray):
	nn = NearestNeighbors(algorithm="brute", metric="cosine" if angular else "l2")
	nn.fit(train)
	return nn.kneighbors(test, n_neighbors=k, return_distance=False)

@dataclass
class Dataset:
	angular: bool
	dim: int
	k: int
	testCount: int
	trainCount: int
	seed: int

	def __post_init__(self):
		self.generated = False

	def generate(self):
		if self.generated:
			return

		self.train = generateData(self.trainCount, self.dim, self.seed)
		self.test = generateData(self.testCount, self.dim, self.seed + 1)
		self.neighbors = getNeighbors(self.angular, self.k, self.test, self.train).astype(np.uint32)
		self.generated = True

	def generateAndWrite(self, name: str, outputDir: Path):
		self.generate()
		outputDir.mkdir(exist_ok=True)
		path = outputDir / name

		with path.with_suffix(".bin").open("wb") as f:
			self.writeBinary(f)

		with hdf.File(path.with_suffix(".hdf5"), "w") as f:
			self.writeHDF(f)

	def writeBinary(self, f: BufferedWriter):
		f.write(struct.pack("<?IIII", self.angular, self.dim, self.k, self.testCount, self.trainCount))
		self.neighbors.flatten().tofile(f)
		self.test.flatten().tofile(f)
		self.train.flatten().tofile(f)

	def writeHDF(self, f: hdf.File):
		f.create_dataset("neighbors", data=self.neighbors)
		f.create_dataset("test", data=self.test)
		f.create_dataset("train", data=self.train)
		f.attrs["distance"] = "angular" if self.angular else "euclidean"
		f.attrs["point_type"] = "float"
