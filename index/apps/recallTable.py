from argparse import ArgumentParser
from helpers.RecallTable import RecallTable
from pathlib import Path

def main():
	parser = ArgumentParser("RECALL TABLE", "Prints recall table of chm_hnsw.Index for given dataset.")
	parser.add_argument("-d", "--dataset", default="angular-10000", help="Dataset name.", required=False)
	args = parser.parse_args()

	try:
		table = RecallTable.fromHDF(Path(__file__).parent / "data" / f"{args.dataset}.hdf5", [10, 50, 100, 500])
		table.run(200, 16, 200)
		print(table)

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
