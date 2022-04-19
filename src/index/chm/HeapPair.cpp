#include "HeapPair.hpp"

namespace chm {
	HeapPair::HeapPair(const uint efConstruction, const uint mMax0) {
		const auto maxLen = std::max(efConstruction, mMax0);
		this->far.reserve(maxLen);
		this->near.reserve(maxLen);
	}

	void HeapPair::prepareHeuristic() {
		this->near.loadFrom(this->far);
	}

	void HeapPair::prepareLowerSearch(const Node& ep) {
		this->far.clear();
		this->far.push(ep);
		this->near.clear();
		this->near.push(ep);
	}

	void HeapPair::push(const float distance, const uint id) {
		this->far.push(distance, id);
		this->near.push(distance, id);
	}

	void HeapPair::reserve(const uint maxLen) {
		this->far.reserve(maxLen);
		this->near.reserve(maxLen);
	}
}
