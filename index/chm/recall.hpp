#pragma once
#include <unordered_set>
#include "KnnResults.hpp"

namespace chm {
	class LabelsWrapper {
		const size_t* const data;

	public:
		const size_t xDim;
		const size_t yDim;

		void fillSet(std::unordered_set<size_t>& set, const size_t x) const;
		const size_t& get(const size_t x, const size_t y) const;
		size_t getComponentCount() const;
		LabelsWrapper(const size_t* const data, const size_t xDim, const size_t yDim);

		#ifdef PYBIND_INCLUDED
			LabelsWrapper(const NumpyArray<size_t>& a);
		#endif
	};

	float getRecall(const size_t* const correctLabels, const size_t* const testedLabels, const size_t queryCount, const size_t k);
	float getRecall(const LabelsWrapper& correctLabels, const LabelsWrapper& testedLabels);

	#ifdef PYBIND_INCLUDED
		float getRecall(const NumpyArray<size_t> correctLabels, const NumpyArray<size_t> testedLabels);
	#endif
}
