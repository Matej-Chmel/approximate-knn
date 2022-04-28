from .runner import AppError
import json
from pathlib import Path

N = "\n"

def configStr(p: Path):
	return f"Config path: {p}{N}"

def getDictValue(d: dict, key: str, expectedType: type, jsonPath: Path, itemType: type = None):
	try:
		val = d[key]
	except KeyError:
		raise AppError(
			f"{configStr(jsonPath)}Missing key: {key}"
		)

	if not isinstance(val, expectedType):
		raise AppError(
			f"{configStr(jsonPath)}Key: {key}{N}"
			f"Expected type: {expectedType}{N}Actual type: {type(val)}"
		)

	if expectedType is list and itemType is not None:
		for item in val:
			if not isinstance(item, itemType):
				raise AppError(
					f"{configStr(jsonPath)}Expected type: list[{itemType}]{N}"
					f"One item was of type: {type(item)}"
				)
	return val

def getRoot(jsonPath: Path, expectedType: type):
	try:
		with jsonPath.open("r", encoding="utf-8") as f:
			obj = json.load(f)

			if not isinstance(obj, expectedType):
				raise AppError(
					f"{configStr(jsonPath)}Root element must be of type {expectedType}. Was {type(obj)}."
				)
			return obj
	except FileNotFoundError:
		raise AppError(f"Could not open {jsonPath}.")
	except PermissionError:
		raise AppError(f"Permission to {jsonPath} denied.")
