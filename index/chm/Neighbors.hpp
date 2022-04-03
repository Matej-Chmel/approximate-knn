#pragma once
#include "Heap.hpp"

namespace chm {
	class Neighbors {
		std::vector<uint>::iterator beginIter;
		std::vector<uint>::iterator count;
		std::vector<uint>::iterator endIter;

	public:
		std::vector<uint>::const_iterator begin() const;
		void clear();
		std::vector<uint>::const_iterator end() const;
		void fillFrom(const FarHeap& h);
		void fillFrom(const FarHeap& h, Node& nearest);
		uint get(const size_t i) const;
		uint len() const;
		Neighbors(const std::vector<uint>::iterator& count);
		void push(const uint id);
	};
}
