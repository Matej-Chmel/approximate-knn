#pragma once
#include "Heap.hpp"

namespace chm {
	struct HeapPair {
		FarHeap far;
		NearHeap near;

		HeapPair(const uint efConstruction, const uint mMax0);
		void prepareHeuristic();
		void prepareLowerSearch(const Node& ep);
		void push(const float distance, const uint id);
		void reserve(const uint maxLen);
	};
}
