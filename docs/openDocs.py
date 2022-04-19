from argparse import ArgumentParser
from pathlib import Path
import shutil
import subprocess
import webbrowser as wb

NO_DOCS = "No documentation exists."

class Args:
	def __init__(self):
		p = ArgumentParser("OPEN_DOCS", description="Opens the documentation website.")
		p.add_argument("-c", "--clean", action="store_true", help="Delete the documentation folder.")
		p.add_argument("-g", "--generate", action="store_true", help="Generate the documentation website.")
		args = p.parse_args()
		self.clean: bool = args.clean
		self.generate: bool = args.generate

def clean(htmlDir: Path):
	try:
		shutil.rmtree(htmlDir)
		print("Documentation deleted.")
	except FileNotFoundError:
		print(NO_DOCS)
	except PermissionError:
		print("Permission denied to delete documentation folder.")

def main():
	args = Args()
	htmlDir = Path(__file__).parent / "html"

	if args.clean:
		clean(htmlDir)
	if args.generate:
		subprocess.call(["doxygen", "Doxyfile"], cwd=Path(__file__).parents[1] / "src" / "index" / "chm")
		print("Documentation generated.")

	indexPath = (htmlDir / "index.html").absolute()

	if indexPath.exists():
		wb.open_new_tab(f"file:///{indexPath}")
	else:
		print(NO_DOCS)

if __name__ == "__main__":
	main()
