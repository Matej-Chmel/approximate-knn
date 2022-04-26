from chmTools.runner import wrapMain
import latexTable as lt
from pathlib import Path

def main():
	figuresDir = Path(__file__).absolute().parents[1] / "figures"
	lt.writeTable(lt.Args(
		algoNames=["new-heuristic", "new-naive"], calcPercent=True, dataset="lastfm-64-dot",
		label="NaiveTab", legend=["Heuristika", "Naivní alg."], output=figuresDir / "NaiveTab.tex"
	))
	lt.writeTable(lt.Args(
		algoNames=["new-prefetch", "new-heuristic"], calcPercent=True, dataset="glove-50-angular",
		label="PrefetchTab", legend=["Asynchronní", "Synchronní"], output=figuresDir / "PrefetchTab.tex"
	))
	lt.writeTable(lt.Args(
		algoNames=["new-prefetch", "hnswlib"], calcPercent=True, dataset="sift-128-euclidean",
		label="BenchmarkTab", legend=["Nová impl.", "Původní impl."], output=figuresDir / "BenchmarkTab.tex"
	))

if __name__ == "__main__":
	wrapMain(main)
