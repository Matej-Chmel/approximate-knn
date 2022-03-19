#ifndef PYBIND_INCLUDED
	#define PYBIND_INCLUDED
#endif

#include "chm/Index.hpp"
#include "chm/recall.hpp"

namespace chm {
	PYBIND11_MODULE(chm_hnsw, m) {
		m.def(
			"getRecall", py::overload_cast<const NumpyArray<uint>, const NumpyArray<uint>>(getRecall),
			py::arg("correctLabels"), py::arg("testedLabels")
		);
		m.doc() = "Python bindings for HNSW index from Matej-Chmel/approximate-knn.";

		py::enum_<SpaceKind>(m, "Space")
			.value("ANGULAR", SpaceKind::ANGULAR)
			.value("EUCLIDEAN", SpaceKind::EUCLIDEAN)
			.value("INNER_PRODUCT", SpaceKind::INNER_PRODUCT);

		py::class_<Index>(m, "Index")
			.def(
				py::init<const size_t, const uint, const uint, const uint, const uint, const SpaceKind>(),
				py::arg("dim"), py::arg("efConstruction"), py::arg("maxCount"),
				py::arg("mMax"), py::arg("seed"), py::arg("spaceKind")
			)
			.def("__str__", [](const Index& h) { return h.getString(); })
			.def("push", py::overload_cast<const NumpyArray<float>>(&Index::push), py::arg("data"))
			.def(
				"query", py::overload_cast<const NumpyArray<float>, const uint>(&Index::query),
				py::arg("data"), py::arg("k")
			)
			.def("setEfSearch", &Index::setEfSearch, py::arg("efSearch"));
	}
}
