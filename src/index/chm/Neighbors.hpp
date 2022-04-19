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
		 * Vrátí konstantní iterátor @ref beginIter.
		 * @return Konstantní iterátor @ref beginIter.
		 */
		std::vector<uint>::const_iterator begin() const;
		/**
		 * Vymaže seznam sousedů.
		 */
		void clear();
		/**
		 * Vrátí konstantní iterátor @ref endIter.
		 * @return Konstantní iterátor @ref endIter.
		 */
		std::vector<uint>::const_iterator end() const;
		/**
		 * Přidá vrcholy z haldy do seznamu.
		 * @param[in] h Halda.
		 */
		void fillFrom(const FarHeap& h);
		/**
		 * Přidá vrcholy z haldy do seznamu a uloží nejbližší vrchol ke zkoumanému prvku z haldy.
		 * @param[in] h Halda.
		 * @param[out] nearest Nejbližší vrchol.
		 */
		void fillFrom(const FarHeap& h, Node& nearest);
		/**
		 * Vrátí identitu souseda na pozici @p i.
		 * @param[in] i Pozice souseda.
		 * @return Identita souseda.
		 */
		uint get(const size_t i) const;
		/**
		 * Vrátí počet sousedů v seznamu.
		 * @return Počet sousedů.
		 */
		uint len() const;
		/**
		 * Konstruktor.
		 * @param[in] count Ukazatel na buňku s počtem sousedů v seznamu.
		 */
		Neighbors(const std::vector<uint>::iterator& count);
		/**
		 * Přidá souseda do seznamu.
		 * @param[in] id Identita nového souseda.
		 */
		void push(const uint id);
	};
}
