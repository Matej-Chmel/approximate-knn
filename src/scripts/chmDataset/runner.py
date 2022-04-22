from typing import Callable

def run(f: Callable):
	try:
		f()
		raise SystemExit(0)
	except Exception as e:
		print("[ERROR]", e)
		raise SystemExit(1)
