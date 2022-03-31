#pragma once
#include <vector>
#include "DistanceInfo.hpp"
#include "types.hpp"

namespace chm {
	enum class SpaceKind {
		ANGULAR,
		EUCLIDEAN,
		INNER_PRODUCT
	};

	class Space {
		uint count;
		std::vector<float> data;
		const size_t dim4;
		const size_t dim16;
		size_t dimLeft;
		DistanceFunction distFunc;
		std::string distName;
		const bool normalize;
		std::vector<float> query;

		float getNorm(const float* const data) const;
		void normalizeData(const float* const data, float* const res) const;

	public:
		const size_t dim;

		uint getCount() const;
		const float* const getData(const uint id) const;
		float getDistance(const uint aID, const uint bID) const;
		float getDistance(const float* const a, const float* const b) const;
		float getDistance(const float* const aData, const uint bID) const;
		std::string getDistanceName() const;
		const float* const getNormalizedQuery(const float* const data);
		bool isEmpty() const;
		void prefetch(const uint id) const;
		void push(const float* const data, const uint count);
		Space(const size_t dim, const SpaceKind kind, const size_t maxCount);
	};
}
