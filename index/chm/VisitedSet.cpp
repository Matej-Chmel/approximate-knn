#include "DistanceFunction.hpp"
#include "VisitedSet.hpp"

namespace chm {
	VisitResult VisitResult::fail() {
		return VisitResult(0, 0, false);
	}

	VisitResult::VisitResult(const uint idx, const uint neighborID, const bool success)
		: idx(idx), neighborID(neighborID), success(success) {}

	bool VisitedSet::insert(const uint id) {
		if(this->v[id])
			return false;

		this->v[id] = true;
		return true;
	}

	VisitResult VisitedSet::insertNext(const Neighbors& N, const uint startIdx) {
		const auto len = N.len();

		for(uint idx = startIdx; idx < len; idx++) {
			const auto id = N.get(idx);

			if(this->insert(id))
				return VisitResult(idx, id, true);
		}

		return VisitResult::fail();
	}

	void VisitedSet::prepare(const uint count, const uint epID) {
		std::fill(this->v.begin(), this->v.begin() + count, false);
		this->v[epID] = true;
	}

	VisitedSet::VisitedSet(const uint maxCount) : v(maxCount, false) {}
}
