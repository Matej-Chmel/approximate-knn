from argparse import ArgumentParser
import run_multiple as m

def main():
	p = ArgumentParser("QUICK_BENCHMARK", description="Runs a quick benchmark.")
	p.add_argument("-a", "--angular", action="store_true", help="Run benchmarks on an angular dataset.")
	args = p.parse_args()
	m.tryRun(m.Config(
		[m.CONFIG_DIR / "quick.yaml"],
		["lastfm-64-dot" if args.angular else "mnist-784-euclidean"],
		dockerWorkers=2, runs=1
	))

if __name__ == "__main__":
	main()
