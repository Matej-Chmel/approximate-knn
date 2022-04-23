from __future__ import absolute_import
import collections
from enum import Enum
import importlib
from pathlib import Path
import yaml

Definition = collections.namedtuple(
	"Definition", [
		"algorithm", "arguments", "constructor", "disabled",
		"docker_tag", "module", "query_argument_groups"
	]
)


def instantiate_algorithm(definition):
	print(
		f"Trying to instantiate {definition.module}.{definition.constructor}({definition.arguments})"
	)
	module = importlib.import_module(definition.module)
	constructor = getattr(module, definition.constructor)
	return constructor(*definition.arguments)


class InstantiationStatus(Enum):
	AVAILABLE = 0
	NO_CONSTRUCTOR = 1
	NO_MODULE = 2


def algorithm_status(definition):
	try:
		module = importlib.import_module(definition.module)
		if hasattr(module, definition.constructor):
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

def get_definitions(
	definition_file: Path, dimension, point_type="float", distance_metric="euclidean", count=10
):
	parsedCfg = _get_definitions(definition_file)

	if point_type != "float":
		print("[ERROR] Only float type is supported.")
		raise SystemExit(1)

	algorithm_definitions = parsedCfg["algos"]
	definitions = []

	for name, configs in algorithm_definitions.items():
		if name.startswith("new"):
			constructor = "ChmHnsw" + {
				"new-avx": "AVX",
				"new-heuristic": "Heuristic",
				"new-no-bit": "NoBitArray",
				"new-no-simd": "NoSIMD",
				"new-naive": "Naive",
				"new-prefetch": "Prefetching",
				"new-sse": "SSE"
			}[name]
			dockerTag = "ann-benchmarks-chm-hnsw"
			module = "ann_benchmarks.algorithms.chm_hnsw"
		else:
			constructor = "HnswLib"
			dockerTag = "ann-benchmarks-hnswlib"
			module = "ann_benchmarks.algorithms.hnswlib"

		for cfg in configs:
			definitions.append(Definition(
				algorithm=name,
				arguments=[cfg["efConstruction"], cfg["mMax"], distance_metric],
				constructor=constructor,
				docker_tag=dockerTag,
				disabled=False,
				module=module,
				query_argument_groups=parsedCfg["efSearch"]
			))

	return definitions
