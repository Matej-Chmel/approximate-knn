from pathlib import Path
from tools.Dataset import Dataset

N = "\n"

def main():
	try:
		dataDir = Path(__file__).parent.parent / "data"
		datasetPath = dataDir / "test.hdf5"
		descPath = dataDir / "testPy.txt"
		Dataset.fromHDF(datasetPath).writeDescription(descPath)
		print(f"Description of dataset {datasetPath} written to {descPath}.")

	except FileNotFoundError:
		print("HDF5 dataset not found. Run datasetGenerator.py first.")

if __name__ == "__main__":
	main()
