from argparse import ArgumentParser
import clean
from chmDataset import runner
from chmDataset.AppError import AppError
from dataclasses import dataclass
from pathlib import Path
import platform
import shutil
import subprocess
import sys

@dataclass
class Args:
	clean: bool
	ignorePythonVersion: bool

def buildBindings(executable: Path, indexDir: Path, repoDir: Path):
	print("Building bindings.")
	shutil.copy(repoDir / "LICENSE", indexDir / "LICENSE")
	subprocess.check_call([executable, "-m", "pip", "install", "."], cwd=indexDir)
	output = subprocess.check_output(
		[executable, "-c", "import chm_hnsw as h; print(h.__doc__)"]
	).decode("utf-8")
	print(f"Module docstring: {output}")
	print("Bindings built.")

def buildNativeLib(executable: Path, indexDir: Path, scriptsDir: Path, srcDir: Path):
	print("Building build system for native library.")
	subprocess.check_call([executable, "formatCMakeTemplates.py"], cwd=scriptsDir)
	cmakeBuildDir = srcDir / "cmakeBuild"
	cmakeBuildDir.mkdir(exist_ok=True)
	subprocess.check_call(["cmake", indexDir.absolute()], cwd=cmakeBuildDir)
	print("Build system for native library built.")

def buildVirtualEnv(repoDir: Path, scriptsDir: Path):
	print("Building virtual environment.")
	subprocess.check_call([sys.executable, "-m", "venv", ".venv"], cwd=repoDir)
	executable = getVirtualEnvExecutable(repoDir)

	if not executable.exists():
		raise AppError("Python virtual environment executable not found.")

	cmdline = [executable, "-m", "pip", "install"]
	subprocess.check_call(cmdline + ["--upgrade", "pip"], cwd=repoDir)
	subprocess.check_call(cmdline + ["-r", "requirements.txt"], cwd=scriptsDir)
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

def generateDatasets(executable: Path, scriptsDir: Path, srcDir: Path):
	print("Generating datasets.")
	subprocess.check_call([executable, "datasetGenerator.py"], cwd=scriptsDir)
	dataDir = srcDir / "data"

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

def runRecallTable(executable: Path, scriptsDir: Path):
	print("Running recall table Python program.")
	subprocess.check_call([executable, "runRecallTable.py"], cwd=scriptsDir)
	print("Recall table run successfully.")

def main():
	args = getArgs()
	cleanProject(args)
	checkPythonVersion(args)

	repoDir = Path(__file__).parents[2]
	srcDir = repoDir / "src"
	indexDir = srcDir / "index"
	scriptsDir = srcDir / "scripts"

	executable = buildVirtualEnv(repoDir, scriptsDir)
	buildNativeLib(executable, indexDir, scriptsDir, srcDir)
	buildBindings(executable, indexDir, repoDir)
	generateDatasets(executable, scriptsDir, srcDir)
	runRecallTable(executable, scriptsDir)
	print("Completed.")

if __name__ == "__main__":
	runner.run(main)
