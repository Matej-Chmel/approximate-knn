#pragma once
#include <algorithm>
#include <utility>
#include <vector>
#include "Node.hpp"

namespace chm {
	/**
	 * Halda.
	 * Šablona Comparator určuje funkci, která porovnává vrcholy haldy.
	 */
	template<class Comparator>
	class Heap {
		/**
		 * Vrcholy haldy.
		 */
		std::vector<Node> nodes;

		/**
		 * Vrátí ukazatel na začátek pole nodes.
		 */
		std::vector<Node>::iterator begin();
		/**
		 * Vrátí ukazatel na konec pole nodes.
		 */
		std::vector<Node>::iterator end();

	public:
		/**
		 * Vrátí ukazatel na začátek pole nodes, přes který nelze pole změnit.
		 */
		std::vector<Node>::const_iterator begin() const noexcept;
		/**
		 * Vymaže haldu.
		 */
		void clear();
		/**
		 * Vrátí ukazatel na konec pole nodes, přes který nelze pole změnit.
		 */
		std::vector<Node>::const_iterator end() const noexcept;
		/**
		 * Konstruktor prázdné haldy.
		 */
		Heap() = default;
		/**
		 * Konstruktor haldy s počátečním vrcholem.
		 */
		Heap(const Node& ep);
		/**
		 * Vrátí počet vrcholů v haldě.
		 */
		size_t len() const;
		/**
		 * Přemístí vrcholy z haldy o do této haldy.
		 */
		template<class OtherComparator> void loadFrom(Heap<OtherComparator>& o);
		/**
		 * Vrátí vrchol v poli nodes na pozici i.
		 */
		const Node& operator[](const size_t i) const;
		/**
		 * Odstraní kořen haldy.
		 */
		void pop();
		/**
		 * Přidá vrchol do haldy.
		 */
		void push(const Node& n);
		/**
		 * Vytvoří vrchol a přidá jej do haldy.
		 */
		void push(const float distance, const uint id);
		/**
		 * Nastaví kapacitu pole nodes.
		 */
		void reserve(const size_t capacity);
		/**
		 * Vrátí odkaz na kořen haldy.
		 */
		const Node& top() const;
	};

	/**
	 * Halda, kde kořenem je vrchol s největší vzdáleností.
	 */
	using FarHeap = Heap<FarComparator>;
	/**
	 * Halda, kde kořenem je vrchol s nejmenší vzdáleností.
	 */
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
