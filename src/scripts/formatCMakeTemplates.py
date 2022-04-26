from chmTools.runner import wrapMain
from pathlib import Path
from SIMDCapability import SIMDCapability

def formatCMakeTemplates(srcDir: Path, templatesDir: Path):
	simd = SIMDCapability()
	arch = simd.getMsvcArchFlag()
	archStr = "" if arch is None else arch
	macros = " ".join(simd.getMacros())
	N = "\n"

	with (srcDir / "index" / "CMakeLists.txt").open("w", encoding="utf-8") as f:
		f.write((templatesDir / "CMake.txt").read_text(encoding="utf-8"
			).replace("@ARCH@", f" {archStr}"
			).replace(
				"@DEFINITIONS@",
				f"{N}target_compile_definitions(chmLib PRIVATE {macros})" if macros else ""
			).replace("@SIMD@", f" {macros}" if macros else ""
		))

def main():
	scriptDir = Path(__file__).absolute().parent
	formatCMakeTemplates(scriptDir.parent, scriptDir / "templates")

if __name__ == "__main__":
	wrapMain(main)
