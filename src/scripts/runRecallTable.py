from chmDataset import runner
from chmDataset.AppError import AppError
from chmDataset.RecallTable import RecallTable, RecallTableConfig
from pathlib import Path

def main():
	srcDir = Path(__file__).parents[1]

	try:
		cfgPath = srcDir / "config" / "recallTableConfig.json"
		cfg = RecallTableConfig.fromJSON(cfgPath)
	except FileNotFoundError:
		raise AppError(f"No configuration file found at {cfgPath}.")
	except KeyError as e:
		raise AppError(f"Configuration file is missing key {e.args[0]}.")

	try:
		table = RecallTable.fromHDF(cfg, srcDir / "data" / f"{cfg.dataset}.hdf5")
		table.run()
		print(table)
	except FileNotFoundError:
		raise AppError("HDF5 dataset not found.")

if __name__ == "__main__":
	runner.run(main)
