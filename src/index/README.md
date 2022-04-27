# chm_hnsw

Python bindings for HNSW index classes from [Matej-Chmel/approximate-knn](https://github.com/Matej-Chmel/approximate-knn).

# Third-party software

This project uses code taken from the following libraries.

- [hnswlib](https://github.com/nmslib/hnswlib/tree/7cc0ecbd43723418f43b8e73a46debbbc3940346), [License](LICENSE_hnswlib).
	- Definition of PORTABLE_ALIGN types in file [DistanceFunction.hpp](chm/DistanceFunction.hpp).
	- Distance metrics in files [euclideanDistance.hpp](chm/euclideanDistance.hpp) and [innerProduct.hpp](chm/innerProduct.hpp). List of changes:
		- Changed conditional compilation macros.
		- User preference considered while selecting SIMD extension.
		- Distance metrics don't compute number of parallelizable space dimensions.
		- Each distance metric is assigned an object which stores pointer to the metric and its name.
