from pathlib import Path
import shutil

def cleanProject():
	repoDir = Path(__file__).parent
	benchmarksDir = Path("benchmarks")
	annBenchmarks = benchmarksDir / "ann_benchmarks"

	for path in [
		"__pycache__", ".venv", "build", "chm_hnsw.egg-info", "cmakeBuild", "data", "dist",
		benchmarksDir / "__pycache__", annBenchmarks / "__pycache__",
		annBenchmarks / "algorithms" / "__pycache__", annBenchmarks / "plotting" / "__pycache__",
		Path("executables", "tools", "__pycache__")
	]:
		deleteDir(repoDir / path)

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
	cleanProject()

if __name__ == "__main__":
	main()
