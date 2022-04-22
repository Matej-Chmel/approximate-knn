from chmDataset import runner
from chmDataset.AppError import AppError
from chmDataset.Dataset import Dataset
from pathlib import Path

N = "\n"

def main():
	try:
		dataDir = Path(__file__).parents[1] / "data"
		datasetPath = dataDir / "test.hdf5"
		descPath = dataDir / "testPy.txt"
		Dataset.fromHDF(datasetPath).writeDescription(descPath)
		print(f"Description of dataset {datasetPath} written to {descPath}.")

	except FileNotFoundError:
		raise AppError("HDF5 dataset not found. Run datasetGenerator.py first.")

if __name__ == "__main__":
	runner.run(main)
