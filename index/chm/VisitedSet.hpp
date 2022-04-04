#pragma once
#include <vector>
#include "Neighbors.hpp"
#include "types.hpp"

namespace chm {
	struct VisitResult {
		uint idx;
		uint neighborID;
		bool success;

		static VisitResult fail();
		VisitResult(const uint idx, const uint neighborID, const bool success);
	};

	class VisitedSet {
		std::vector<bool> v;

	public:
		bool insert(const uint id);
		VisitResult insertNext(const Neighbors& N, const uint startIdx);
		void prepare(const uint count, const uint epID);
		VisitedSet(const uint maxCount);
	};
}
