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
		 * @param[in] efConstruction Počet prvků,
		 * ze kterých index vybírá nové sousedy při vkládání nového prvku.
		 * @see Configuration::efConstruction
		 * @param[in] mMax0 Maximální počet sousedů prvku na vrstvě 0.
		 * @see Configuration::mMax0
		 */
		HeapPair(const uint efConstruction, const uint mMax0);
		/**
		 * Přemístí vrcholy z haldy @ref far do haldy @ref near.
		 */
		void prepareHeuristic();
		/**
		 * Odstraní všechny vrcholy z obou hald
		 * a vloží do nich počáteční vrchol pro prohledání nižší vrstvy.
		 * @param[in] ep Počáteční vrchol.
		 */
		void prepareLowerSearch(const Node& ep);
		/**
		 * Vytvoří nový vrchol a vloží jej do obou hald.
		 * @param[in] distance Vzdálenost vrcholu od zkoumaného prvku.
		 * @param[in] id Identita prvku, který je zastupován vytvořeným vrcholem.
		 */
		void push(const float distance, const uint id);
		/**
		 * Upraví kapacitu obou hald.
		 * @param[in] maxLen Nová kapacita.
		 * Nejvyšší počet vrcholů, který kdy může obsahovat jedna halda.
		 */
		void reserve(const uint maxLen);
	};
}
