#include "recall.hpp"

namespace chm {
	void LabelsWrapper::fillSet(std::unordered_set<uint>& set, const size_t x) const {
		set.clear();

		for(size_t y = 0; y < this->yDim; y++)
			set.insert(this->get(x, y));
	}

	uint LabelsWrapper::get(const size_t x, const size_t y) const {
		return this->data[x * this->yDim + y];
	}

	size_t LabelsWrapper::getComponentCount() const {
		return this->xDim * this->yDim;
	}

	LabelsWrapper::LabelsWrapper(const uint* const data, const size_t xDim, const size_t yDim)
		: data(data), xDim(xDim), yDim(yDim) {}

	float getRecall(const uint* const correctLabels, const uint* const testedLabels, const size_t queryCount, const size_t k) {
		return getRecall(LabelsWrapper(correctLabels, queryCount, k), LabelsWrapper(testedLabels, queryCount, k));
	}

	float getRecall(const LabelsWrapper& correctLabels, const LabelsWrapper& testedLabels) {
		size_t hits = 0;
		std::unordered_set<uint> correctSet;
		correctSet.reserve(correctLabels.yDim);

		for(size_t x = 0; x < correctLabels.xDim; x++) {
			correctLabels.fillSet(correctSet, x);

			for(size_t y = 0; y < correctLabels.yDim; y++)
				if(correctSet.find(testedLabels.get(x, y)) != correctSet.end())
					hits++;
		}

		return float(hits) / (correctLabels.getComponentCount());
	}

	#ifdef PYBIND_INCLUDED

		LabelsWrapper::LabelsWrapper(const NumpyArray<uint>& a)
			: data(getNumpyPtr(a)), xDim(getNumpyXDim(a)), yDim(getNumpyYDim(a)) {}

		float getRecall(const NumpyArray<uint> correctLabels, const NumpyArray<uint> testedLabels) {
			return getRecall(LabelsWrapper(correctLabels), LabelsWrapper(testedLabels));
		}

	#endif
}
