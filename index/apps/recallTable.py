from helpers.RecallTable import RecallTable
from pathlib import Path

def main():
	try:
		table = RecallTable.fromHDF(Path(__file__).parent / "data" / "test.hdf5", [3, 4, 6])
		table.run(10, 5, 100)
		print(table)

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
