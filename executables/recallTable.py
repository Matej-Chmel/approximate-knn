from dataclasses import dataclass
import json
from pathlib import Path
from tools.RecallTable import RecallTable

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
	repoDir = Path(__file__).parent.parent

	try:
		configPath = repoDir / "config" / "recallTableConfig.json"
		config = Config.fromJSON(configPath)
	except FileNotFoundError:
		return print(f"No configuration file found at {configPath}.")
	except KeyError as e:
		return print(f"Configuration file is missing key {e.args[0]}.")

	try:
		table = RecallTable.fromHDF(repoDir / "data" / f"{config.dataset}.hdf5", config.efSearch)
		table.run(config.efConstruction, config.mMax, config.seed)
		print(table)

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
