import subprocess
import sys
from typing import Callable

class AppError(Exception):
	pass

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
