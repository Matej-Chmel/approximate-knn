#include "VisitedSet.hpp"

namespace chm {
	VisitResult VisitResult::fail() {
		return VisitResult(0, 0, false);
	}

	VisitResult::VisitResult(const size_t idx, const uint neighborID, const bool success)
		: idx(idx), neighborID(neighborID), success(success) {}
}
