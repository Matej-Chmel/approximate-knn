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

def main():
	checkPythonVersion()
	repoDir = Path(__file__).parent
	srcDir = repoDir / "src"
	configDir = srcDir / "config"
	scriptsDir = srcDir / "scripts"

	subprocess.check_call([sys.executable, "buildProject.py"], cwd=scriptsDir)
	executable = getVirtualEnvExecutable(repoDir)
	subprocess.check_call(
		[
			executable, "runBenchmarks.py",
			"-a", (configDir / "100k-small.yaml").absolute(), "-d", "random-s-100-angular",
			"-f", "-r", "1", "-w", str(max(1, multiprocessing.cpu_count() - 1))
		],
		cwd=scriptsDir
	)
	subprocess.check_call([executable, "openDocs.py"], cwd=repoDir / "docs")

def run():
	try:
		main()
		raise SystemExit(0)
	except subprocess.CalledProcessError:
		raise SystemExit(1)
	except Exception as e:
		print("[ERROR]", e)
		raise SystemExit(1)

if __name__ == "__main__":
	run()
