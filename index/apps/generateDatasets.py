from tools.Dataset import Dataset
from pathlib import Path

def main():
	dataDir = Path(__file__).parent / "data"

	Dataset(False, 4, 3, 5, 20, 100).generateAndWrite("test", dataDir)
	Dataset(True, 4, 10, 100, 10000, 100).generateAndWrite("angular-10000", dataDir)
	Dataset(True, 16, 10, 200, 20000, 150).generateAndWrite("angular-20000", dataDir)
	Dataset(False, 4, 10, 100, 10000, 100).generateAndWrite("euclidean-10000", dataDir)
	Dataset(False, 16, 10, 200, 20000, 150).generateAndWrite("euclidean-20000", dataDir)

if __name__ == "__main__":
	main()
