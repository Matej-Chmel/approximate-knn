from pathlib import Path
from SIMDCapability import SIMDCapability

def formatCMakeTemplates(srcDir: Path):
	simd = SIMDCapability()
	arch = simd.getMsvcArchFlag()
	archStr = "" if arch is None else arch
	macros = " ".join(simd.getMacros())
	N = "\n"

	with (srcDir / "index" / "CMakeLists.txt").open("w", encoding="utf-8") as f:
		f.write((srcDir / "templates" / "CMake.txt").read_text(encoding="utf-8"
			).replace("@ARCH@", f" {archStr}"
			).replace(
				"@DEFINITIONS@",
				f"{N}target_compile_definitions(chmLib PRIVATE {macros})" if macros else ""
			).replace("@SIMD@", f" {macros}" if macros else ""
		))

def main():
	formatCMakeTemplates(Path(__file__).parents[2])

if __name__ == "__main__":
	main()
