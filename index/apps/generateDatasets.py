from apps.Dataset import Dataset
from pathlib import Path

def main():
	Dataset(False, 4, 3, 10, 1000, 100).generateAndWrite("test", Path(__file__).parent / "data")

if __name__ == "__main__":
	main()
