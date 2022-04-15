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
		 * Počet čísel v poli upperLayers, které je nutné přeskočit
		 * pro přístup k seznamu sousedů na vrstvě o jednu úroveň výše.
		 */
		const uint maxLen;
		/**
		 * Počet čísel v poli layer0, které je nutné přeskočit
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
		 */
		Connections(const uint maxNodeCount, const uint mMax, const uint mMax0);
		/**
		 * Vrátí seznam sousedů prvku s identitou id na vrstvě lc.
		 */
		Neighbors getNeighbors(const uint id, const uint lc);
		/**
		 * Vytvoří seznamy sousedů prvku s identitou id v poli upperLayers.
		 */
		void init(const uint id, const uint level);
	};
}
