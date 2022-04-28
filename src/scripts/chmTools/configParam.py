from argparse import ArgumentParser
from pathlib import Path

def getConfigPath(prog: str, description: str, srcDir: Path):
	p = ArgumentParser(prog=prog, description=description)
	p.add_argument(
		"-c", "--config", default=srcDir / "config" / "config.json",
		help="Path to JSON configuration file.", type=Path
	)
	res: Path = p.parse_args().config
	return res.absolute().resolve(strict=False)
