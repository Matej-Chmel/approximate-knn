from argparse import ArgumentParser
from clean import deleteDir
from dataclasses import dataclass
from pathlib import Path
import platform
import subprocess
import sys

@dataclass
class Args:
	buildCmake: bool
	buildBenchmarks: bool
	buildBindings: bool
	ignorePythonVersion: bool
	rebuild: bool

	def __post_init__(self):
		self.scriptDir = Path(__file__).parent

	def getVirtualEnvExecutable(self):
		args.scriptDir / ".venv"

	def shouldSkip(self, p: Path):
		return not self.rebuild and p.exists() and containsAnyFile(p)

def buildCmake(args: Args):
	cmakeBuildDir = args.scriptDir / "cmakeBuild"

	if args.shouldSkip(cmakeBuildDir):
		return print("Skipping CMake build.")

	if cmakeBuildDir.exists():
		deleteDir(cmakeBuildDir)
		print("Delete cmakeBuild.")

	cmakeBuildDir.mkdir(exist_ok=True)
	subprocess.call(["cmake", "./.."], cwd=cmakeBuildDir)
	print("CMake target built.")

def buildVirtualEnv(args: Args):
	venvDir = args.scriptDir / ".venv"

	if args.shouldSkip(venvDir):
		print("Skipping virtual environment.")
		return True

	if not isPython39():
		print(f"Current Python version is {pythonVersionStr()}. Tested only with Python 3.9.")

		if not args.ignorePythonVersion:
			print("Skipping virtual environment.")
			return False

		print("Ignoring Python version.")

	subprocess.call([sys.executable, "-m", "venv", ".venv"], cwd=args.scriptDir)
	executable = venvDir / "Scripts" / "python"

	if isWindows():
		executable = executable.with_suffix(".exe")

	subprocess.call([executable, "-m", "pip", "install", "--upgrade", "pip"], cwd=args.scriptDir)
	print("Virtual environment built.")

def containsAnyFile(p: Path):
	return any(f.is_file() for f in p.rglob("*"))

def getArgs():
	p = ArgumentParser(prog="BUILD", description="Builds the project.")
	p.register("type", "bool", lambda v: v.lower() in ["1", "true", "y", "yes"])
	p.add_argument(
		"--buildCmake", type="bool", default=True,
		help="Build CMake target in cmakeBuild directory."
	)
	p.add_argument(
		"--buildBenchmarks", type="bool", default=True,
		help="Build virtual environment with requirements from benchmarks directory."
	)
	p.add_argument(
		"--buildBindings", type="bool", default=True,
		help="Build virtual environment and install Python bindings."
	)
	p.add_argument(
		"--ignorePythonVersion", action="store_true", default=False,
		help="Don't check Python version to be equal to 3.9."
	)
	p.add_argument("-r", "--rebuild", action="store_true", default=False, help="Rebuild all targets.")
	args = p.parse_args()
	return Args(args.buildCmake, args.buildBenchmarks, args.buildBindings, args.ignorePythonVersion, args.rebuild)

def isPython39():
	return sys.version_info.major == 3 and sys.version_info.minor == 9

def isWindows():
	return platform.system() == "Windows"

def pythonVersionStr():
	return f"{sys.version_info.major}.{sys.version_info.minor}"

def main():
	args = getArgs()
	buildCmake(args)
	buildVirtualEnv(args)

if __name__ == "__main__":
	main()
