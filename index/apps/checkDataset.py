from io import TextIOWrapper
import h5py as hdf
from pathlib import Path

N = "\n"

def run():
	dataDir = Path(__file__).parent / "data"

	with hdf.File(dataDir / "test.hdf5", "r") as hdfFile, (dataDir / "testPy.txt").open("w", encoding="utf-8") as outFile:
		outFile.write(f"angular: {1 if hdfFile.attrs['distance'] == 'angular' else 0}{N}")
		outFile.write(f"dim: {hdfFile['train'].shape[1]}{N}")
		outFile.write(f"k: {hdfFile['neighbors'].shape[1]}{N}")
		outFile.write(f"testCount: {hdfFile['test'].shape[0]}{N}")
		outFile.write(f"trainCount: {hdfFile['train'].shape[0]}{N}")
		writeHdfDataset(hdfFile, "neighbors", outFile)
		writeHdfDataset(hdfFile, "test", outFile, ".6f")
		writeHdfDataset(hdfFile, "train", outFile, ".6f")

def writeHdfDataset(hdfFile: hdf.File, key: str, outFile: TextIOWrapper, format: str = ""):
	arr = hdfFile[key][:]
	outFile.write(f"{key}[length {arr.shape[0] * arr.shape[1]}]{N}")

	for i in arr.flat:
		outFile.write(f"{i:{format}}{N}")

def main():
	try:
		run()
	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
