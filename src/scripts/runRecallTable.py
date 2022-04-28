from chmTools.module import AppError, RecallTable, RecallTableConfig, wrapMain
from pathlib import Path

def computeTable(cfg: RecallTableConfig, dataDir: Path):
	try:
		datasetPath = dataDir / f"{cfg.dataset}.hdf5"
		table = RecallTable.fromHDF(cfg, datasetPath)
		table.run()
		return table
	except FileNotFoundError:
		raise AppError(f'Dataset "{datasetPath}" not found.')

def main():
	srcDir = Path(__file__).absolute().parents[1]

	try:
		cfgPath = srcDir / "config" / "config.json"
		configs = RecallTableConfig.listFromJSON(cfgPath)
	except FileNotFoundError:
		raise AppError(f"No configuration file found at {cfgPath}.")

	dataDir = srcDir / "data"
	tables = [computeTable(cfg, dataDir) for cfg in configs]
	print("\n\n".join(map(str, tables)))

if __name__ == "__main__":
	wrapMain(main)
