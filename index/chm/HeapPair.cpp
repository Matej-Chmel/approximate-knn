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

	void HeapPair::prepareLowerSearch(const Node& ep, FarHeap& W) {
		this->near.clear();
		this->near.push(ep);
		W.clear();
		W.push(ep);
	}

	void HeapPair::push(const float distance, const uint id, FarHeap& W) {
		this->near.push(distance, id);
		W.push(distance, id);
	}

	void HeapPair::reserve(const uint maxLen, FarHeap& W) {
		this->near.reserve(maxLen);
		W.reserve(maxLen);
	}
}
