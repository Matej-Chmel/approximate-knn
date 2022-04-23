import subprocess
import sys
from typing import Callable

class AppError(Exception):
	pass

def checkInsideVenv():
	if not insideVenv():
		print("[ERROR] This script must be run inside a virtual environment.")
		raise SystemExit(1)

def insideVenv():
	return sys.base_prefix != sys.prefix

def wrapMain(f: Callable):
	try:
		f()
		raise SystemExit(0)
	except subprocess.CalledProcessError:
		raise SystemExit(1)
	except Exception as e:
		print("[ERROR]", e)
		raise SystemExit(1)
