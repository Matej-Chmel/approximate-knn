#include "VisitedSet.hpp"

namespace chm {
	bool VisitedSet::insert(const uint id) {
		if (this->v[id])
			return false;

		this->v[id] = true;
		return true;
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
