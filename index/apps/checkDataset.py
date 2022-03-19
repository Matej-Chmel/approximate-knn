from tools.Dataset import Dataset
from pathlib import Path

N = "\n"

def main():
	try:
		dataDir = Path(__file__).parent / "data"
		datasetPath = dataDir / "test.hdf5"
		descPath = dataDir / "testPy.txt"
		Dataset.fromHDF(datasetPath).writeDescription(descPath)
		print(f"Description of dataset {datasetPath} written to {descPath}.")

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
