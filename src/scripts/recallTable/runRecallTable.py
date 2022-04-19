from pathlib import Path
from RecallTable import RecallTable, RecallTableConfig

def main():
	repoDir = Path(__file__).parent.parent

	try:
		cfgPath = repoDir / "config" / "recallTableConfig.json"
		cfg = RecallTableConfig.fromJSON(cfgPath)
	except FileNotFoundError:
		return print(f"No configuration file found at {cfgPath}.")
	except KeyError as e:
		return print(f"Configuration file is missing key {e.args[0]}.")

	try:
		table = RecallTable.fromHDF(cfg, repoDir / "data" / f"{cfg.dataset}.hdf5")
		table.run()
		print(table)

	except FileNotFoundError:
		print("HDF5 dataset not found.")

if __name__ == "__main__":
	main()
