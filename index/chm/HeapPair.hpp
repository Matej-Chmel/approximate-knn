#pragma once
#include "Heap.hpp"

namespace chm {
	/**
	 * Dvojice hald třídící vrcholy podle opačných pravidel.
	 */
	struct HeapPair {
		/**
		* Halda, kde kořenem je vrchol s největší vzdáleností.
		*/
		FarHeap far;
		/**
		* Halda, kde kořenem je vrchol s nejmenší vzdáleností.
		*/
		NearHeap near;

		/**
		 * Konstruktor.
		 */
		HeapPair(const uint efConstruction, const uint mMax0);
		/**
		 * Přemístí vrcholy z haldy far do haldy near.
		 */
		void prepareHeuristic();
		/**
		 * Odstraní všechny vrcholy z obou hald
		 * a vloží do nich počáteční vrchol ep.
		 */
		void prepareLowerSearch(const Node& ep);
		/**
		 * Vytvoří nový vrchol a vloží jej do obou hald.
		 */
		void push(const float distance, const uint id);
		/**
		 * Upraví kapacitu obou hald na maxLen.
		 */
		void reserve(const uint maxLen);
	};
}
