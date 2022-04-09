from glob import glob
import numpy as np
import platform
import pybind11
import setuptools
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext
import sys
import tempfile
from scripts.SIMDCapability import SIMDCapability

MSVC_QUOTE = r'\\"'

def addPreprocessorMacro(name: str, compilerType: str, opts: list[str], val: str = None):
	if compilerType == "msvc":
		cmd = f"/D{name}"

		if val is not None:
			cmd += f"={MSVC_QUOTE}{val}{MSVC_QUOTE}"

		opts.append(cmd)
	else:
		cmd = f"-D{name}"

		if val is not None:
			cmd += f'="{val}"'

		opts.append(cmd)

def addSIMDMacros(compilerType: str, opts: list[str]):
	simd = SIMDCapability()

	for macro in simd.getMacros():
		addPreprocessorMacro(macro, compilerType, opts)

	if compilerType == "msvc":
		arch = simd.getMsvcArchFlag()

		if arch is not None:
			opts.append(arch)

class BuildExt(build_ext):
	"""A custom build extension for adding compiler-specific options."""
	c_opts = {
		"msvc": ["/EHsc", "/O2"],
		"unix": ["-O3"]
	}
	link_opts = {
		"msvc": [],
		"unix": []
	}

	c_opts["unix"].append("-march=native")

	if sys.platform == "darwin":
		if platform.machine() == "arm64":
			c_opts["unix"].remove("-march=native")
		c_opts["unix"] += ["-stdlib=libc++", "-mmacosx-version-min=10.7"]
		link_opts["unix"] += ["-stdlib=libc++", "-mmacosx-version-min=10.7"]

	def build_extensions(self):
		ct = self.compiler.compiler_type
		opts = self.c_opts.get(ct, [])
		cppStandard = "-std=c++17"

		if ct == "msvc":
			cppStandard = "/std:c++17"
		elif ct == "unix" and hasFlag(self.compiler, "-fvisibility=hidden"):
			opts.append("-fvisibility=hidden")

		opts.append(cppStandard)
		addPreprocessorMacro("PYBIND_INCLUDED", ct, opts)
		addPreprocessorMacro("VERSION_INFO", ct, opts, self.distribution.get_version())
		addSIMDMacros(ct, opts)

		for ext in self.extensions:
			ext.extra_compile_args.extend(opts)
			ext.extra_link_args.extend(self.link_opts.get(ct, []))

		build_ext.build_extensions(self)

# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def hasFlag(compiler, flag):
	"""Returns a boolean indicating whether a flag name is supported on the specified compiler."""

	with tempfile.NamedTemporaryFile("w", suffix=".cpp") as f:
		f.write("int main(int argc, char **argv){return 0;}")

		try:
			compiler.compile([f.name], extra_postargs=[flag])
		except setuptools.distutils.errors.CompileError:
			return False

	return True

def main():
	desc = "Custom implementation of HNSW index."
	setup(
		author="Matěj Chmel",
		cmdclass={"build_ext": BuildExt},
		description=desc,
		ext_modules=[
			Extension(
				"chm_hnsw",
				[*glob("./index/chm/*.cpp"), "./index/bindings.cpp"],
				include_dirs=[
					pybind11.get_include(),
					np.get_include(),
					"./index/"
				],
				language="c++"
			)
		],
		install_requires=["numpy"],
		long_description=desc,
		name="chm_hnsw",
		url="https://github.com/Matej-Chmel/approximate-knn",
		version="0.0.18",
		zip_safe=False
	)

if __name__ == "__main__":
	main()
