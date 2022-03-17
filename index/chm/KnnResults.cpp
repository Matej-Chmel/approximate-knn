#include <stdexcept>
#include "KnnResults.hpp"

namespace chm {
	constexpr auto WRONG_DIM = "Data must be 1D or 2D array.";
	constexpr auto WRONG_FEATURES = "Number of features doesn't equal to number of dimensions.";

	FloatArray::FloatArray(const float* const data, const uint count, const size_t dim) : data(data), count(count) {
		if(this->count % dim)
			throw std::runtime_error(WRONG_FEATURES);
	}

	KnnResults::~KnnResults() {
		if(this->owningData) {
			delete[] this->distances;
			delete[] this->labels;
		}
	}

	KnnResults::KnnResults(const size_t count, const size_t k)
		: count(count), distances(new float[this->count * k]), k(k),
		labels(new size_t[this->count * this->k]), owningData(true) {}

	void KnnResults::setData(const size_t queryIdx, const size_t neighborIdx, const float distance, const size_t label) {
		this->distances[queryIdx * this->k + neighborIdx] = distance;
		this->labels[queryIdx * this->k + neighborIdx] = label;
	}

	#ifdef PYBIND_INCLUDED

		void freeWhenDone(void* d) {
			delete[] d;
		}

		DataInfo::DataInfo(const NumpyArray<float>& data, const size_t dim) : data(getNumpyPtr(data)), count(getNumpyXDim(data)) {
			const auto buf = data.request();

			if (buf.ndim == 2) {
				if (buf.shape[1] != dim)
					throw std::runtime_error(WRONG_FEATURES);
			}
			else if (buf.ndim != 1)
				throw std::runtime_error(WRONG_DIM);
		}

		py::tuple KnnResults::makeTuple() const {
			this->owningData = false;
			return py::make_tuple(
				py::array_t<size_t>(
					{this->count, this->k},
					{this->k * sizeof(size_t), sizeof(size_t)},
					this->labels,
					py::capsule(this->labels, freeWhenDone)
				),
				py::array_t<float>(
					{this->count, this->k},
					{this->k * sizeof(float), sizeof(float)},
					this->distances,
					py::capsule(this->distances, freeWhenDone)
				)
			);
		}

	#endif
}
