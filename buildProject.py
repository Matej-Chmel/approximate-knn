from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path
import platform
import subprocess
import sys
import scripts.clean as clean
from scripts.SIMDCapability import SIMDCapability

class AppError(Exception):
	pass

@dataclass
class Args:
	clean: bool
	ignorePythonVersion: bool

def buildBindings(executable: Path, repoDir: Path):
	print("Building bindings.")
	subprocess.call([executable, "-m", "pip", "install", "."], cwd=repoDir)
	output = subprocess.check_output(
		[executable, "-c", "import chm_hnsw as h; print(h.__doc__)"]
	).decode("utf-8")
	print(f"Module docstring: {output}")
	print("Bindings built.")

def buildNativeLib(repoDir: Path):
	print("Building build system for native library.")
	cmakeBuildDir = repoDir / "cmakeBuild"
	cmakeBuildDir.mkdir(exist_ok=True)
	formatCMakeTemplates(repoDir)
	subprocess.call(["cmake", "./.."], cwd=cmakeBuildDir)
	print("Build system for native library built.")

def buildVirtualEnv(repoDir: Path):
	print("Building virtual environment.")
	subprocess.call([sys.executable, "-m", "venv", ".venv"], cwd=repoDir)
	executable = getVirtualEnvExecutable(repoDir)

	if not executable.exists():
		raise AppError("Python virtual environment executable not found.")

	subprocess.call([executable, "-m", "pip", "install", "--upgrade", "pip"], cwd=repoDir)
	subprocess.call([executable, "-m", "pip", "install", "-r", "benchmarks/requirements.txt"], cwd=repoDir)
	subprocess.call([executable, "-m", "pip", "install", "-r", "scripts/requirements.txt"], cwd=repoDir)
	print("Virtual environment built.")
	return executable

def checkPythonVersion(args: Args):
	if not args.ignorePythonVersion and (sys.version_info.major != 3 or sys.version_info.minor != 9):
		raise AppError("Python 3.9 is required to build the project.")

def cleanProject(args: Args):
	if args.clean:
		print("Cleaning project.")
		clean.cleanProject(False)
		print("Project cleaned.")

def formatCMakeTemplates(repoDir: Path):
	simd = SIMDCapability()
	arch = simd.getMsvcArchFlag()
	archStr = "" if arch is None else arch
	macros = " ".join(simd.getMacros())
	templatesDir = repoDir / "cmakeTemplates"

	with (repoDir / "index" / "CMakeLists.txt").open("w", encoding="utf-8") as f:
		f.write((templatesDir / "index.txt").read_text(encoding="utf-8"
			).replace(
				"@DEFINITIONS@",
				f"target_compile_definitions(chmLib PRIVATE {macros})" if macros else ""
		))

	with (repoDir / "CMakeLists.txt").open("w", encoding="utf-8") as f:
		f.write((templatesDir / "root.txt").read_text(encoding="utf-8"
			).replace("@ARCH@", f" {archStr}"
			).replace("@SIMD@", f" {macros}" if macros else ""
		))

def generateDatasets(executable: Path, repoDir: Path):
	print("Generating datasets.")
	subprocess.call([executable, "datasetGenerator.py"], cwd=repoDir / "executables")
	dataDir = repoDir / "data"

	if not dataDir.exists():
		raise AppError("Data directory wasn't created.")
	if not list(dataDir.glob("*.bin")):
		raise AppError("No binary datasets were generated.")
	if not list(dataDir.glob("*.hdf5")):
		raise AppError("No HDF5 datasets were generated.")

	print("Datasets generated.")

def getArgs():
	p = ArgumentParser(
		"BUILD",
		"Builds Python virtual environment, bindings and build system for native library."
	)
	p.add_argument(
		"-c", "--clean", action="store_true",
		help="Cleans the project before the build."
	)
	p.add_argument(
		"-i", "--ignorePythonVersion", action="store_true",
		help="Skips Python version check."
	)
	args = p.parse_args()
	return Args(args.clean, args.ignorePythonVersion)

def getVirtualEnvExecutable(repoDir: Path):
	p = repoDir / ".venv" / "Scripts" / "python"

	if onWindows():
		p = p.with_suffix(".exe")

	return p

def onWindows():
	return platform.system().strip().lower() == "windows"

def run():
	args = getArgs()
	cleanProject(args)
	checkPythonVersion(args)
	repoDir = Path(__file__).parent
	executable = buildVirtualEnv(repoDir)
	generateDatasets(executable, repoDir)
	buildNativeLib(repoDir)
	buildBindings(executable, repoDir)
	runRecallTable(executable, repoDir)
	print("Completed.")

def runRecallTable(executable: Path, repoDir: Path):
	print("Running recall table Python program.")
	subprocess.call([executable, "recallTable.py"], cwd=repoDir / "executables")
	print("Recall table run successfully.")

def main():
	try:
		run()
	except AppError as e:
		print(f"[APP ERROR] {e}")
	except subprocess.SubprocessError as e:
		print(f"[SUBPROCESS ERROR] {e}")

if __name__ == "__main__":
	main()
