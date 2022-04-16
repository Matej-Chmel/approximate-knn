#pragma once
#include "Neighbors.hpp"

namespace chm {
	/**
	 * Ukládá seznamy sousedů.
	 */
	class Connections {
		/**
		 * Seznamy sousedů prvků na vrstvě 0.
		 */
		std::vector<uint> layer0;
		/**
		 * Počet položek v poli upperLayers, které je nutné přeskočit
		 * pro přístup k seznamu sousedů na vrstvě o jednu úroveň výše.
		 */
		const uint maxLen;
		/**
		 * Počet položek v poli layer0, které je nutné přeskočit
		 * pro přístup k seznamu sousedů prvku s identitou o jedna vyšší.
		 */
		const uint maxLen0;
		/**
		 * Seznamy sousedů prvků na vyšších vrstvách než vrstva 0.
		 */
		std::vector<std::vector<uint>> upperLayers;

	public:
		/**
		 * Konstruktor.
		 * @param[in] maxNodeCount Maximální počet prvků v indexu.
		 * @param[in] mMax Maximální počet sousedů prvku na vrstvě vyšší než vrstva 0.
		 * @param[in] mMax0 Maximální počet sousedů prvku na vrstvě 0.
		 */
		Connections(const uint maxNodeCount, const uint mMax, const uint mMax0);
		/**
		 * Vrátí seznam sousedů prvku @p id na vrstvě @p lc.
		 * @param[in] id Identita prvku.
		 * @param[in] lc Číselné označení vrstvy.
		 * @return Seznam sousedů prvku @p id na vrstvě @p lc.
		 */
		Neighbors getNeighbors(const uint id, const uint lc);
		/**
		 * Vytvoří seznamy sousedů prvku @p id v poli @ref upperLayers.
		 * @param[in] id Identita prvku.
		 * @param[in] level Úroveň prvku. Číslo nejvyšší vrstvy, která obsahuje vrchol prvku.
		 */
		void init(const uint id, const uint level);
	};
}
