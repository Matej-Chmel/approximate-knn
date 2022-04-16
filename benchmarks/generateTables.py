import latexTable as lt
from pathlib import Path

def main():
	tablesDir = Path(__file__).parent
	tablesDir.mkdir(exist_ok=True)
	lt.tryRun(lt.Args(
		algoNames=["chm-hnsw-heuristic", "chm-hnsw-naive"], calcPercent=True, dataset="lastfm-64-dot",
		label="NaiveTab", legend=["Heuristika", "Naivní alg."], output=tablesDir / "NaiveTab.tex"
	))
	lt.tryRun(lt.Args(
		algoNames=["chm-hnsw-prefetching", "chm-hnsw-heuristic"], calcPercent=True, dataset="glove-50-angular",
		label="PrefetchTab", legend=["Asynchronní", "Synchronní"], output=tablesDir / "PrefetchTab.tex"
	))
	lt.tryRun(lt.Args(
		algoNames=["chm-hnsw-prefetching", "hnswlib"], calcPercent=True, dataset="sift-128-euclidean",
		label="BenchmarkTab", legend=["Nová impl.", "Původní impl."], output=tablesDir / "BenchmarkTab.tex"
	))

if __name__ == "__main__":
	main()
