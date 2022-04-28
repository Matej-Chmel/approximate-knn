from argparse import ArgumentParser
from chmTools.module import (
	getConfigPath, getDictValue, getRoot,
	RecallTable, RecallTableConfig, wrapMain
)
from pathlib import Path

def computeTable(cfg: RecallTableConfig, dataDir: Path, datasets: list, jsonPath: Path):
	table = RecallTable.getTable(cfg, dataDir, datasets, jsonPath)
	table.run()
	return table

def main():
	srcDir = Path(__file__).absolute().parents[1]

	jsonPath = getConfigPath("RECALL_TABLE", "Computes recall table.", srcDir)
	configObj = getRoot(jsonPath, dict)
	datasets = getDictValue(configObj, "datasets", list, jsonPath, dict)
	indexConfigs = getDictValue(configObj, "index", list, jsonPath, dict)

	configs = RecallTableConfig.listFromJSONArray(indexConfigs, jsonPath)
	dataDir = srcDir / "data"
	tables = [computeTable(cfg, dataDir, datasets, jsonPath) for cfg in configs]
	print("\n\n".join(map(str, tables)))

if __name__ == "__main__":
	wrapMain(main)
