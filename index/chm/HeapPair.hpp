#pragma once
#include "Heap.hpp"

namespace chm {
	struct HeapPair {
		FarHeap far;
		NearHeap near;

		HeapPair(const uint efConstruction, const uint mMax0);
		void prepareHeuristic();
		void prepareLowerSearch(const Node& ep, FarHeap& W);
		void push(const float distance, const uint id, FarHeap& W);
		void reserve(const uint maxLen, FarHeap& W);
	};
}
