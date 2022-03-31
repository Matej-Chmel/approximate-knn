#pragma once
#include <vector>
#include "Neighbors.hpp"
#include "types.hpp"

namespace chm {
	struct FindResult {
		uint idx;
		uint neighborID;
		bool success;
	};

	class VisitedSet {
		std::vector<bool> v;

	public:
		FindResult findNext(const Neighbors& N, const uint startIdx) const;
		void insert(const uint id);
		void prepare(const uint count, const uint epID);
		VisitedSet(const uint maxCount);
	};
}
