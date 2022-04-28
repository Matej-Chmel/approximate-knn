from chmTools.runner import checkInsideVenv, wrapMain
checkInsideVenv()

from chmTools.configParam import getConfigPath
from chmTools.Dataset import Dataset
from chmTools.jsonTypeCheck import getDictValue, getRoot
from pathlib import Path

def main():
	srcDir = Path(__file__).absolute().parents[1]
	configPath = getConfigPath("DATASET_GENERATOR", "Generates datasets.", srcDir)
	dataDir = srcDir / "data"

	for obj in getDictValue(getRoot(configPath, dict), "datasets", list, configPath, dict):
		Dataset.fromDict(obj, configPath
		).generateAndWrite(getDictValue(obj, "name", str, configPath), dataDir)

if __name__ == "__main__":
	wrapMain(main)
