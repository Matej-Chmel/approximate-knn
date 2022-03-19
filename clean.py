from pathlib import Path
import shutil

def cleanProject():
	scriptDir = Path(__file__).parent

	for path in [
		".venv", "build", "chm_hnsw.egg-info", "cmakeBuild", "dist",
		Path("index") / "apps" / "tools" / "__pycache__"
	]:
		deleteDir(scriptDir / path)

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
