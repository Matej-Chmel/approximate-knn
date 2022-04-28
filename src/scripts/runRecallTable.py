from argparse import ArgumentParser
from chmTools.module import AppError, RecallTable, RecallTableConfig, wrapMain
from pathlib import Path

def computeTable(cfg: RecallTableConfig, dataDir: Path):
	try:
		datasetPath = dataDir / f"{cfg.dataset}.hdf5"
		table = RecallTable.fromHDF(cfg, datasetPath)
		table.run()
		return table
	except FileNotFoundError:
		raise AppError(f'HDF5 dataset "{datasetPath}" not found. Run datasetGenerator.py first.')

def getConfigPath(srcDir: Path):
	p = ArgumentParser("RECALL_TABLE", description="Computes recall table.")
	p.add_argument(
		"-c", "--config", default=srcDir / "config" / "config.json",
		help="Path to JSON configuration file.", type=Path
	)
	return p.parse_args().config

def main():
	srcDir = Path(__file__).absolute().parents[1]
	configs = RecallTableConfig.listFromJSON(getConfigPath(srcDir))
	dataDir = srcDir / "data"
	tables = [computeTable(cfg, dataDir) for cfg in configs]
	print("\n\n".join(map(str, tables)))

if __name__ == "__main__":
	wrapMain(main)
