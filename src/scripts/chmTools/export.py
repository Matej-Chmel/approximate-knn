import pandas as pd
from pathlib import Path
from .runner import AppError
import subprocess
import sys

def getExportedData(recompute: bool):
	srcDir = Path(__file__).parents[2]
	csvPath = (srcDir / "figures" / "data.csv").absolute()

	if recompute or not csvPath.exists():
		csvPath.parent.mkdir(exist_ok=True, parents=True)
		try:
			subprocess.check_call(
				[sys.executable, "data_export.py", "-o", csvPath] + (["-r"] if recompute else []),
				cwd=srcDir / "benchmarks"
			)
		except subprocess.CalledProcessError as e:
			raise AppError(f"Error while exporting data: {e}") from e

	return pd.read_csv(csvPath, sep=",")
