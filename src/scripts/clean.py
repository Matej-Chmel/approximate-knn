from argparse import ArgumentParser
from chmTools.runner import insideVenv, wrapMain
from pathlib import Path
import shutil

def cleanProject(deleteVenv: bool, deleteResults: bool = False):
	repo = Path(__file__).parents[2]

	src = repo / "src"
	benchmarks = src / "benchmarks"
	annBenchmarks = benchmarks / "ann_benchmarks"
	index = src / "index"
	scripts = src / "scripts"

	deleteFile(annBenchmarks / "__pycache__")
	deleteFile(annBenchmarks / "algorithms" / "__pycache__")
	deleteFile(annBenchmarks / "plotting" / "__pycache__")
	deleteFile(benchmarks / "__pycache__")
	deleteFile(index / "build")
	deleteFile(index / "chm_hnsw.egg-info")
	deleteFile(index / "CMakeLists.txt")
	deleteFile(index / "dist")
	deleteFile(repo / "__pycache__")
	deleteFile(scripts / "__pycache__")
	deleteFile(scripts / "chmDataset" / "__pycache__")
	deleteFile(src / "cmakeBuild")
	deleteFile(src / "data")

	if deleteVenv:
		deleteFile(repo / ".venv")
	if deleteResults:
		deleteFile(benchmarks / "results")
		deleteFile(scripts / "benchmarks_time.txt")
		deleteFile(src / "figures")
		deleteFile(src / "website")

def deleteFile(p: Path):
	def delete(p: Path):
		if p.exists():
			try:
				if p.is_dir():
					shutil.rmtree(p)
				elif p.is_file():
					p.unlink()
				else:
					return "Unknown file type"
				return "Deleted"
			except PermissionError:
				return "Permission denied"
		else:
			return "Does not exist"
	print(f"[{p}] {delete(p)}")

def main():
	p = ArgumentParser("CLEAN", description="Cleans the project.")
	p.add_argument(
		"-r", "--results", action="store_true",
		help="Delete benchmark results, generated figures and website."
	)
	args = p.parse_args()
	cleanProject(not insideVenv(), args.results)

if __name__ == "__main__":
	wrapMain(main)
