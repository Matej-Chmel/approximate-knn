#pragma once
#include <algorithm>
#include <utility>
#include <vector>
#include "Node.hpp"

namespace chm {
	template<class Comparator>
	class Heap {
		std::vector<Node> nodes;

		std::vector<Node>::iterator begin();
		std::vector<Node>::iterator end();

	public:
		std::vector<Node>::const_iterator begin() const noexcept;
		void clear();
		std::vector<Node>::const_iterator end() const noexcept;
		Heap() = default;
		Heap(const Node& ep);
		size_t len() const;
		template<class OtherComparator> void loadFrom(Heap<OtherComparator>& o);
		const Node& operator[](const size_t i) const;
		void pop();
		void push(const Node& n);
		void push(const float distance, const uint id);
		void reserve(const size_t capacity);
		const Node& top() const;
	};

	using FarHeap = Heap<FarComparator>;
	using NearHeap = Heap<NearComparator>;

	template<class Comparator>
	inline std::vector<Node>::iterator Heap<Comparator>::begin() {
		return this->nodes.begin();
	}

	template<class Comparator>
	inline std::vector<Node>::iterator Heap<Comparator>::end() {
		return this->nodes.end();
	}

	template<class Comparator>
	inline std::vector<Node>::const_iterator Heap<Comparator>::begin() const noexcept {
		return this->nodes.cbegin();
	}

	template<class Comparator>
	inline void Heap<Comparator>::clear() {
		this->nodes.clear();
	}

	template<class Comparator>
	inline std::vector<Node>::const_iterator Heap<Comparator>::end() const noexcept {
		return this->nodes.cend();
	}

	template<class Comparator>
	inline Heap<Comparator>::Heap(const Node& ep) {
		this->nodes.emplace_back(ep.distance, ep.id);
	}

	template<class Comparator>
	inline size_t Heap<Comparator>::len() const {
		return this->nodes.size();
	}

	template<class Comparator>
	template<class OtherComparator>
	inline void Heap<Comparator>::loadFrom(Heap<OtherComparator>& o) {
		this->clear();

		for(const auto& n : std::as_const(o))
			this->push(n);
	}

	template<class Comparator>
	inline const Node& Heap<Comparator>::operator[](const size_t i) const {
		return this->nodes[i];
	}

	template<class Comparator>
	inline void Heap<Comparator>::pop() {
		std::pop_heap(this->begin(), this->end(), Comparator());
		this->nodes.pop_back();
	}

	template<class Comparator>
	inline void Heap<Comparator>::push(const Node& n) {
		this->push(n.distance, n.id);
	}

	template<class Comparator>
	inline void Heap<Comparator>::push(const float distance, const uint id) {
		this->nodes.emplace_back(distance, id);
		std::push_heap(this->begin(), this->end(), Comparator());
	}

	template<class Comparator>
	inline void Heap<Comparator>::reserve(const size_t capacity) {
		this->nodes.reserve(capacity);
	}

	template<class Comparator>
	inline const Node& Heap<Comparator>::top() const {
		return this->nodes.front();
	}
}
