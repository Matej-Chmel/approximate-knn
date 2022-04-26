from argparse import ArgumentParser
from chmTools.module import AppError, Dataset, wrapMain
from pathlib import Path

N = "\n"

def writeDescription(datasetName: str):
	try:
		dataDir = Path(__file__).absolute().parents[1] / "data"
		datasetPath = (dataDir / datasetName).with_suffix(".hdf5")
		descPath = (dataDir / datasetName).with_suffix(".py.txt")
		Dataset.fromHDF(datasetPath).writeDescription(descPath)
		print(f"Description of dataset {datasetPath} written to {descPath}.")

	except FileNotFoundError:
		raise AppError(f'HDF5 dataset "{datasetName}" not found. Run datasetGenerator.py first.')

def main():
	p = ArgumentParser("DATASET_TO_TEXT", description="Converts dataset to a text file.")
	p.add_argument("-n", "--name", default="angular-small", help="Dataset name.")
	writeDescription(p.parse_args().name)

if __name__ == "__main__":
	wrapMain(main)
