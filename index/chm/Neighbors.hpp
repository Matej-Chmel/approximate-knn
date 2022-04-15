#pragma once
#include "Heap.hpp"

namespace chm {
	/**
	 * Seznam sousedů.
	 */
	class Neighbors {
		/**
		 * Ukazatel na začátek seznamu.
		 */
		std::vector<uint>::iterator beginIter;
		/**
		 * Ukazatel na buňku s počtem sousedů.
		 */
		std::vector<uint>::iterator count;
		/**
		 * Ukazatel na konec seznamu.
		 */
		std::vector<uint>::iterator endIter;

	public:
		/**
		 * Vrátí konstantní iterátor beginIter.
		 */
		std::vector<uint>::const_iterator begin() const;
		/**
		 * Vymaže seznam sousedů.
		 */
		void clear();
		/**
		 * Vrátí konstantní iterátor endIter.
		 */
		std::vector<uint>::const_iterator end() const;
		/**
		 * Přidá vrcholy z haldy h do seznamu.
		 */
		void fillFrom(const FarHeap& h);
		/**
		 * Přidá vrcholy z haldy h do seznamu a do parametru nearest uloží nejbližší vrchol.
		 */
		void fillFrom(const FarHeap& h, Node& nearest);
		/**
		 * Vrátí souseda na pozici i.
		 */
		uint get(const size_t i) const;
		/**
		 * Vrátí počet sousedů.
		 */
		uint len() const;
		/**
		 * Konstruktor.
		 */
		Neighbors(const std::vector<uint>::iterator& count);
		/**
		 * Přidá souseda do seznamu.
		 */
		void push(const uint id);
	};
}
