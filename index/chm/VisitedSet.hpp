#pragma once
#include <vector>
#include "Neighbors.hpp"
#include "types.hpp"

namespace chm {
	struct VisitedResult {
		uint idx;
		uint neighborID;
		bool success;

		static VisitedResult fail();
		VisitedResult(const uint idx, const uint neighborID, const bool success);
	};

	class VisitedSet {
		std::vector<bool> v;

	public:
		VisitedResult findNext(const Neighbors& N, const uint startIdx) const;
		bool insert(const uint id);
		bool isMarked(const uint id) const;
		void mark(const uint id);
		void prefetch(const uint id) const;
		void prepare(const uint count, const uint epID);
		VisitedSet(const uint maxCount);
	};
}
