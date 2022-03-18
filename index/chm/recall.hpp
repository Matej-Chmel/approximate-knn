#pragma once
#include <unordered_set>
#include "KnnResults.hpp"

namespace chm {
	class LabelsWrapper {
		const uint* const data;

	public:
		const size_t xDim;
		const size_t yDim;

		void fillSet(std::unordered_set<uint>& set, const size_t x) const;
		uint get(const size_t x, const size_t y) const;
		size_t getComponentCount() const;
		LabelsWrapper(const uint* const data, const size_t xDim, const size_t yDim);

		#ifdef PYBIND_INCLUDED
			LabelsWrapper(const NumpyArray<uint>& a);
		#endif
	};

	float getRecall(const uint* const correctLabels, const uint* const testedLabels, const size_t queryCount, const size_t k);
	float getRecall(const LabelsWrapper& correctLabels, const LabelsWrapper& testedLabels);

	#ifdef PYBIND_INCLUDED
		float getRecall(const NumpyArray<uint> correctLabels, const NumpyArray<uint> testedLabels);
	#endif
}
