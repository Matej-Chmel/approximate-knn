#ifndef PYBIND_INCLUDED
	#define PYBIND_INCLUDED
#endif

#include "chm/Index.hpp"
#include "chm/recall.hpp"

namespace chm {
	template<class T>
	void bindIndex(py::module_& m, const char* const name) {
		py::class_<Index<T>>(m, name)
			.def(
				py::init<
					const size_t, const uint, const uint, const uint,
					const uint, const SIMDType, const SpaceKind>(),
				py::arg("dim"), py::arg("maxCount"),
				py::arg("efConstruction") = 200, py::arg("mMax") = 16, py::arg("seed") = 100,
				py::arg("SIMD") = SIMDType::NONE, py::arg("space") = SpaceKind::EUCLIDEAN
			)
			.def("__str__", [](const Index<T>& h) { return h.getString(); })
			.def(
				"push",
				py::overload_cast<const NumpyArray<float>>(&Index<T>::push),
				py::arg("data")
			)
			.def(
				"queryBatch",
				py::overload_cast<const NumpyArray<float>, const uint>(
					&Index<T>::queryBatch
				),
				py::arg("data"), py::arg("k") = 10
			)
			.def("setEfSearch", &Index<T>::setEfSearch, py::arg("efSearch"));
	}

	PYBIND11_MODULE(chm_hnsw, m) {
		m.def("getIndexTemplate", getIndexTemplate, py::arg("s"));
		m.def(
			"getRecall", py::overload_cast<const NumpyArray<uint>, const NumpyArray<uint>>(getRecall),
			py::arg("correctLabels"), py::arg("testedLabels")
		);
		m.def("getSIMDType", getSIMDType, py::arg("s"));
		m.doc() = "Python bindings for HNSW index classes from Matej-Chmel/approximate-knn.";

		py::enum_<IndexTemplate>(m, "IndexTemplate")
			.value("HEURISTIC", IndexTemplate::HEURISTIC)
			.value("NAIVE", IndexTemplate::NAIVE)
			.value("NO_BIT_ARRAY", IndexTemplate::NO_BIT_ARRAY)
			.value("PREFETCHING", IndexTemplate::PREFETCHING);

		py::enum_<SIMDType>(m, "SIMDType")
			.value("AVX", SIMDType::AVX)
			.value("AVX512", SIMDType::AVX512)
			.value("BEST", SIMDType::BEST)
			.value("NONE", SIMDType::NONE)
			.value("SSE", SIMDType::SSE);

		py::enum_<SpaceKind>(m, "Space")
			.value("ANGULAR", SpaceKind::ANGULAR)
			.value("EUCLIDEAN", SpaceKind::EUCLIDEAN)
			.value("INNER_PRODUCT", SpaceKind::INNER_PRODUCT);

		bindIndex<HeuristicTemplate>(m, "HeuristicIndex");
		bindIndex<NaiveTemplate>(m, "NaiveIndex");
		bindIndex<NoBitArrayTemplate>(m, "NoBitArrayIndex");
		bindIndex<PrefetchingTemplate>(m, "PrefetchingIndex");
	}
}
