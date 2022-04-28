from chmTools.runner import checkInsideVenv, wrapMain
checkInsideVenv()

from argparse import ArgumentParser
from chmTools.Dataset import Dataset
from chmTools.jsonTypeCheck import getDictValue, getRoot
from pathlib import Path

def getConfigPath(srcDir: Path):
	p = ArgumentParser("DATASET_GENERATOR", description="Generates datasets.")
	p.add_argument(
		"-c", "--config", default=srcDir / "config" / "config.json",
		help="Path to JSON configuration file.", type=Path
	)
	return p.parse_args().config

def main():
	srcDir = Path(__file__).absolute().parents[1]
	configPath = getConfigPath(srcDir)
	dataDir = srcDir / "data"

	for obj in getDictValue(getRoot(configPath, dict), "datasets", list, configPath, dict):
		Dataset.fromDict(obj, configPath
		).generateAndWrite(getDictValue(obj, "name", str, configPath), dataDir)

if __name__ == "__main__":
	wrapMain(main)
