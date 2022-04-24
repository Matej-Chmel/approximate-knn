from __future__ import absolute_import
from dataclasses import dataclass
from enum import Enum
import importlib
from pathlib import Path
import yaml

@dataclass
class Definition:
	algorithm: str
	buildArgs: list
	constructor: str
	dockerTag: str
	disabled: bool
	efSearchValues: list[int]
	module: str

	@property
	def efConstruction(self):
		return self.buildArgs[0]

	def getFilename(self, efSearch: int):
		return f"{self.metric}_e_{self.efConstruction}_m_{self.mMax}_ef_{efSearch}.hdf5"

	@property
	def metric(self):
		return self.buildArgs[2]

	@property
	def mMax(self):
		return self.buildArgs[1]

def instantiate_algorithm(d: Definition):
	print(
		f"Trying to instantiate {d.module}.{d.constructor}("
		f"efConstruction={d.efConstruction}, mMax={d.mMax}, metric={d.metric})"
	)
	module = importlib.import_module(d.module)
	constructor = getattr(module, d.constructor)
	return constructor(d.efConstruction, d.mMax, d.metric)

class InstantiationStatus(Enum):
	AVAILABLE = 0
	NO_CONSTRUCTOR = 1
	NO_MODULE = 2

def algorithm_status(d: Definition):
	try:
		module = importlib.import_module(d.module)
		if hasattr(module, d.constructor):
			return InstantiationStatus.AVAILABLE
		else:
			return InstantiationStatus.NO_CONSTRUCTOR
	except ImportError:
		return InstantiationStatus.NO_MODULE

def _get_definitions(definition_file: Path):
	with definition_file.open("r", encoding="utf-8") as f:
		return yaml.load(f, yaml.SafeLoader)

def list_algorithms(definition_file):
	definitions = _get_definitions(definition_file)

	print("The following algorithms are supported...")
	for a in definitions["algos"]:
		print(a)

def get_unique_algorithms(definition_file):
	definitions = _get_definitions(definition_file)
	return list(sorted(set(a for a in definitions["algos"])))

def get_definitions(definition_file: Path, point_type: str, metric: str):
	parsedCfg = _get_definitions(definition_file)

	if point_type != "float":
		print("[ERROR] Only float type is supported.")
		raise SystemExit(1)

	definitions = []

	for algo in parsedCfg["algos"]:
		if algo.startswith("new"):
			constructor = "ChmHnsw" + {
				"new-avx": "AVX",
				"new-heuristic": "Heuristic",
				"new-no-bit": "NoBitArray",
				"new-no-simd": "NoSIMD",
				"new-naive": "Naive",
				"new-prefetch": "Prefetching",
				"new-sse": "SSE"
			}[algo]
			dockerTag = "ann-benchmarks-chm-hnsw"
			module = "ann_benchmarks.algorithms.chm_hnsw"
		else:
			constructor = "HnswLib"
			dockerTag = "ann-benchmarks-hnswlib"
			module = "ann_benchmarks.algorithms.hnswlib"

		for cfg in parsedCfg["build"]:
			definitions.append(Definition(
				algorithm=algo,
				buildArgs=[cfg["efConstruction"], cfg["mMax"], metric],
				constructor=constructor,
				dockerTag=dockerTag,
				disabled=False,
				efSearchValues=parsedCfg["efSearch"],
				module=module
			))

	return definitions
