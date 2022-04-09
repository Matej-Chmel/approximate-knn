from pathlib import Path
import shutil

def cleanProject(deleteVenv: bool):
	repoDir = Path(__file__).parent.parent
	benchmarksDir = Path("benchmarks")
	annBenchmarks = benchmarksDir / "ann_benchmarks"

	for path in [
		"__pycache__", "build", "chm_hnsw.egg-info", "cmakeBuild", "data", "dist",
		benchmarksDir / "__pycache__", annBenchmarks / "__pycache__",
		annBenchmarks / "algorithms" / "__pycache__", annBenchmarks / "plotting" / "__pycache__",
		Path("executables", "tools", "__pycache__"), Path("scripts", "__pycache__")
	]:
		deleteDir(repoDir / path)

	if deleteVenv:
		deleteDir(repoDir / ".venv")

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
