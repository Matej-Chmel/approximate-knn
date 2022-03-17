#pragma once
#include <vector>
#include "types.hpp"

namespace chm {
	class VisitedSet {
		std::vector<bool> v;

	public:
		bool insert(const uint id);
		void prepare(const uint count, const uint entryID);
		VisitedSet(const uint maxCount);
	};
}
