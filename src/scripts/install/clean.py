from pathlib import Path
import shutil

def cleanProject(deleteVenv: bool):
	repo = Path(__file__).parents[3]

	src = repo / "src"
	benchmarks = src / "benchmarks"
	annBenchmarks = benchmarks / "ann_benchmarks"
	index = src / "index"
	scripts = src / "scripts"

	deleteDir(annBenchmarks / "__pycache__")
	deleteDir(annBenchmarks / "algorithms" / "__pycache__")
	deleteDir(annBenchmarks / "plotting" / "__pycache__")
	deleteDir(benchmarks / "__pycache__")
	deleteDir(index / "build")
	deleteDir(index / "chm_hnsw.egg-info")
	deleteDir(index / "dist")
	deleteDir(repo / "__pycache__")
	deleteDir(src / "cmakeBuild")
	deleteDir(src / "data")
	deleteDir(scripts / "benchmarks" / "__pycache__")
	deleteDir(scripts / "install" / "__pycache__")
	deleteDir(scripts / "latex" / "__pycache__")
	deleteDir(scripts / "recallTable" / "__pycache__")

	if deleteVenv:
		deleteDir(repo / ".venv")

def deleteDir(p: Path):
	pathStr = f"[{p}] "

	if p.exists():
		try:
			shutil.rmtree(p)
			print(f"{pathStr}Deleted.")
		except PermissionError:
			print(f"{pathStr}Permission denied.")
	else:
		print(f"{pathStr}Does not exist.")

def main():
	cleanProject(True)

if __name__ == "__main__":
	main()