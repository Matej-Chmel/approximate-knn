from dataclasses import dataclass
from tools.RecallTable import RecallTable
import json
from pathlib import Path

@dataclass
class Config:
	dataset: str
	efConstruction: int
	efSearch: list[int]
	mMax: int
	seed: int

	@classmethod
	def fromJSON(cls, p: Path):
		with p.open("r", encoding="utf-8") as f:
			obj = json.load(f)
			return Config(obj["dataset"], obj["efConstruction"], obj["efSearch"], obj["mMax"], obj["seed"])

def main():
	scriptDir = Path(__file__).parent

	try:
		configPath = scriptDir / "config" / "recallTableConfig.json"
		config = Config.fromJSON(configPath)
	except FileNotFoundError:
		return print(f"No configuration file found at {configPath}.")
	except KeyError as e:
		return print(f"Configuration file is missing key {e.args[0]}.")

	try:
		table = RecallTable.fromHDF(Path(__file__).parent / "data" / f"{config.dataset}.hdf5", config.efSearch)
		table.run(config.efConstruction, config.mMax, config.seed)
		print(table)

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
