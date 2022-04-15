#pragma once
#include "types.hpp"

namespace chm {
	/**
	 * Výchozí hodnota parametru vyhledávání efSearch.
	 */
	constexpr uint DEFAULT_EF_SEARCH = 10;

	/**
	 * Konfigurace indexu.
	 */
	class Configuration {
		/**
		 * Počet prvků, ze kterých index vybírá výsledky při zpracování dotazu.
		 */
		uint efSearch;

	public:
		/**
		 * Počet prvků, ze kterých index vybírá nové sousedy při vkládání prvku.
		 */
		const uint efConstruction;
		/**
		 * Maximální počet sousedů prvku na vrstvách vyšších než vrstva 0.
		 */
		const uint mMax;
		/**
		 * Maximální počet sousedů prvku na vrstvě 0.
		 * Dvojnásobek mMax.
		 */
		const uint mMax0;

		/**
		 * Konstruktor.
		 */
		Configuration(const uint efConstruction, const uint mMax);
		/**
		 * Vrátí hodnotu efSearch.
		 */
		uint getEfSearch() const;
		/**
		 * Vrátí vyšší hodnotu při výběru mezi efSearch a k.
		 */
		uint getMaxEf(const uint k) const;
		/**
		 * Vrátí optimální hodnotu parametru stavby mL.
		 */
		double getML() const;
		/**
		 * Nastaví hodnotu efSearch.
		 */
		void setEfSearch(const uint efSearch);
	};
}
