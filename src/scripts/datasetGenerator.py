from chmTools.module import AppError, Dataset, wrapMain
import json
from pathlib import Path

def generateDatasets():
	srcDir = Path(__file__).parents[1]
	dataDir = srcDir / "data"

	with (srcDir / "config" / "datasetGeneratorConfig.json").open("r", encoding="utf-8") as f:
		arr = json.load(f)

	for obj in arr:
		writeDataset(obj, dataDir)

def writeDataset(obj: dict, dataDir: Path):
	Dataset(
		obj["angular"], obj["dim"], obj["k"], obj["testCount"], obj["trainCount"], obj["seed"]
	).generateAndWrite(obj["name"], dataDir)

def main():
	try:
		generateDatasets()
	except FileNotFoundError:
		raise AppError("Could not open configuration file.")
	except KeyError as e:
		raise AppError(f"Missing key {e.args[0]}")
	except PermissionError:
		raise AppError("Permission denied.")

if __name__ == "__main__":
	wrapMain(main)
