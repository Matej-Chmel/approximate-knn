#include "DistanceFunction.hpp"
#include "VisitedSet.hpp"

namespace chm {
	VisitedResult VisitedResult::fail() {
		return VisitedResult(0, 0, false);
	}

	VisitedResult::VisitedResult(const uint idx, const uint neighborID, const bool success)
		: idx(idx), neighborID(neighborID), success(success) {}

	VisitedResult VisitedSet::findNext(const Neighbors& N, const uint startIdx) const {
		const auto len = N.len();

		for(uint idx = startIdx; idx < len; idx++) {
			const auto id = N.get(idx);

			if(!this->isMarked(id))
				return VisitedResult(idx, id, true);
		}

		return VisitedResult::fail();
	}

	bool VisitedSet::insert(const uint id) {
		if(this->isMarked(id))
			return false;

		this->mark(id);
		return true;
	}

	bool VisitedSet::isMarked(const uint id) const {
		return this->v[id];
	}

	void VisitedSet::mark(const uint id) {
		this->v[id] = true;
	}

	void VisitedSet::prefetch(const uint id) const {
		#if defined(SSE_CAPABLE)
			// _mm_prefetch(reinterpret_cast<const char*>(&this->v[id]), _MM_HINT_T0);
		#endif
	}

	void VisitedSet::prepare(const uint count, const uint epID) {
		std::fill(this->v.begin(), this->v.begin() + count, false);
		this->v[epID] = true;
	}

	VisitedSet::VisitedSet(const uint maxCount) : v(maxCount, false) {}
}
