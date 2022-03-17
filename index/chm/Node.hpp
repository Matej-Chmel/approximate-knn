#pragma once
#include "types.hpp"

namespace chm {
	struct Node {
		float distance;
		uint id;

		Node();
		Node(const float distance, const uint id);
	};

	struct FarComparator {
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance < b.distance;
		}
	};

	struct NearComparator {
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance > b.distance;
		}
	};
}
