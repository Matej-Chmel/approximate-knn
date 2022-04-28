#pragma once
#include "chm/Node.hpp"
#include "chm/Space.hpp"

namespace chm {
	class Bruteforce {
		const uint k;
		std::vector<Node> nodes;
		Space space;

		void queryOne(const std::vector<float>& v, std::vector<uint>& neighbors, const size_t queryIdx);

	public:
		Bruteforce(
			const size_t dim, const uint k, const size_t maxCount,
			const SIMDType simdType, const SpaceKind spaceKind
		);
		void push(const std::vector<float>& v);
		void query(const std::vector<float>& v, std::vector<uint>& neighbors);
	};

	struct NodeCmp {
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance < b.distance;
		}
	};
}
