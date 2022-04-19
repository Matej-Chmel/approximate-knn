import multiprocessing
from pathlib import Path
import platform
import subprocess
import sys

class AppError(Exception):
	pass

def checkPythonVersion():
	if sys.version_info.major != 3 or sys.version_info.minor != 9:
		raise AppError("Python 3.9 is required to build the project.")

def getVirtualEnvExecutable(repoDir: Path):
	p = repoDir / ".venv" / "Scripts" / "python"

	if platform.system().strip().lower() == "windows":
		p = p.with_suffix(".exe")
	if not p.exists():
		raise AppError(f"Virtual environment not found at {p}.")
	return p

def run():
	checkPythonVersion()
	repoDir = Path(__file__).parent
	srcDir = repoDir / "src"
	configDir = srcDir / "config"
	scriptsDir = srcDir / "scripts"

	subprocess.call([sys.executable, "buildProject.py"], cwd=scriptsDir / "install")
	executable = getVirtualEnvExecutable(repoDir)
	subprocess.call(
		[
			executable, "runBenchmarks.py",
			"-a", (configDir / "100k.yaml").absolute(), "-d", "random-s-100-angular",
			"-f", "-r", "1", "-w", str(max(1, multiprocessing.cpu_count() - 1))
		],
		cwd=scriptsDir / "benchmarks"
	)
	subprocess.call([executable, "openDocs.py"], cwd=repoDir / "docs")

def main():
	try:
		run()
	except AppError as e:
		print("[APP ERROR]", e)

if __name__ == "__main__":
	main()
