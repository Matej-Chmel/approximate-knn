import chm_hnsw
import inspect

def main():
	print("\n".join(dir(chm_hnsw)))
	print("\n".join(dir(chm_hnsw.Index)))
	print("\n".join(dir(chm_hnsw.Space)))

if __name__ == "__main__":
	main()
