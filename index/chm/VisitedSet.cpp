#include "VisitedSet.hpp"

namespace chm {
	FindResult VisitedSet::findNext(const Neighbors& N, const uint startIdx) const {
		const auto len = N.len();

		for(uint i = startIdx; i < len; ++i) {
			const auto id = N.get(i);

			if(!this->v[id])
				return {i, id, true};
		}

		return {0, 0, false};
	}

	void VisitedSet::insert(const uint id) {
		this->v[id] = true;
	}

	void VisitedSet::prepare(const uint count, const uint epID) {
		this->v.clear();
		this->v.assign(count, false);
		this->v[epID] = true;
	}

	VisitedSet::VisitedSet(const uint maxCount) {
		this->v.resize(maxCount);
	}
}
