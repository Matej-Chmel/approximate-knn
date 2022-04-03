from chm_hnsw import getSIMDType, SIMDType
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
	simdType: SIMDType

	@classmethod
	def fromJSON(cls, p: Path):
		with p.open("r", encoding="utf-8") as f:
			obj = json.load(f)
			return Config(
				obj["dataset"], obj["efConstruction"], obj["efSearch"], obj["mMax"], obj["seed"],
				SIMDType.NONE if "SIMD" not in obj or obj["SIMD"] is None else getSIMDType(obj["SIMD"])
			)

def main():
	repoDir = Path(__file__).parent.parent

	try:
		cfgPath = repoDir / "config" / "recallTableConfig.json"
		cfg = Config.fromJSON(cfgPath)
	except FileNotFoundError:
		return print(f"No configuration file found at {cfgPath}.")
	except KeyError as e:
		return print(f"Configuration file is missing key {e.args[0]}.")

	try:
		table = RecallTable.fromHDF(repoDir / "data" / f"{cfg.dataset}.hdf5", cfg.efSearch)
		table.run(cfg.efConstruction, cfg.mMax, cfg.seed, cfg.simdType)
		print(table)

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
