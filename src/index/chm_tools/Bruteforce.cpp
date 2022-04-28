#include <algorithm>
#include "Bruteforce.hpp"

namespace chm {
	void Bruteforce::queryOne(
		const std::vector<float>& v, std::vector<uint>& neighbors, const size_t queryIdx
	) {
		const auto& queryData = v.data() + queryIdx * this->space.dim;

		for(auto& n : this->nodes)
			n.distance = this->space.getDistance(queryData, n.id);

		std::sort(this->nodes.begin(), this->nodes.end(), NodeCmp());

		for(size_t i = 0; i < this->k; i++)
			neighbors[queryIdx * this->k + i] = this->nodes[i].id;
	}

	Bruteforce::Bruteforce(
		const size_t dim, const uint k, const size_t maxCount,
		const SIMDType simdType, const SpaceKind spaceKind
	) : k(k), space(dim, spaceKind, maxCount, simdType) {

		this->nodes.reserve(maxCount);
	}

	void Bruteforce::push(const std::vector<float>& v) {
		const auto len = uint(v.size() / this->space.dim);
		const auto prevCount = uint(this->nodes.size());

		for(uint i = 0; i < len; i++)
			this->nodes.emplace_back(0.f, prevCount + i);

		this->space.push(v.data(), uint(len));
	}

	void Bruteforce::query(const std::vector<float>& v, std::vector<uint>& neighbors) {
		const auto len = v.size() / this->space.dim;

		for(size_t i = 0; i < len; i++)
			this->queryOne(v, neighbors, i);
	}
}
