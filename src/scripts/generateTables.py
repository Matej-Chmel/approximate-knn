from chmDataset import runner
import latexTable as lt
from pathlib import Path

def main():
	figuresDir = Path(__file__).parents[1] / "figures"
	figuresDir.mkdir(exist_ok=True)
	lt.writeTable(lt.Args(
		algoNames=["chm-hnsw-heuristic", "chm-hnsw-naive"], calcPercent=True, dataset="lastfm-64-dot",
		label="NaiveTab", legend=["Heuristika", "Naivní alg."], output=figuresDir / "NaiveTab.tex"
	))
	lt.writeTable(lt.Args(
		algoNames=["chm-hnsw-prefetching", "chm-hnsw-heuristic"], calcPercent=True, dataset="glove-50-angular",
		label="PrefetchTab", legend=["Asynchronní", "Synchronní"], output=figuresDir / "PrefetchTab.tex"
	))
	lt.writeTable(lt.Args(
		algoNames=["chm-hnsw-prefetching", "hnswlib"], calcPercent=True, dataset="sift-128-euclidean",
		label="BenchmarkTab", legend=["Nová impl.", "Původní impl."], output=figuresDir / "BenchmarkTab.tex"
	))

if __name__ == "__main__":
	runner.run(main)
