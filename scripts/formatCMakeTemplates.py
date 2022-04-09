from pathlib import Path
from SIMDCapability import SIMDCapability

def formatCMakeTemplates(repoDir: Path):
	simd = SIMDCapability()
	arch = simd.getMsvcArchFlag()
	archStr = "" if arch is None else arch
	macros = " ".join(simd.getMacros())
	N = "\n"
	templatesDir = repoDir / "cmakeTemplates"

	with (repoDir / "index" / "CMakeLists.txt").open("w", encoding="utf-8") as f:
		f.write((templatesDir / "index.txt").read_text(encoding="utf-8"
			).replace(
				"@DEFINITIONS@",
				f"{N}target_compile_definitions(chmLib PRIVATE {macros})" if macros else ""
		))

	with (repoDir / "CMakeLists.txt").open("w", encoding="utf-8") as f:
		f.write((templatesDir / "root.txt").read_text(encoding="utf-8"
			).replace("@ARCH@", f" {archStr}"
			).replace("@SIMD@", f" {macros}" if macros else ""
		))

def main():
	formatCMakeTemplates(Path(__file__).parent.parent)

if __name__ == "__main__":
	main()
