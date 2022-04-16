#pragma once
#include "types.hpp"

namespace chm {
	/**
	 * Výchozí hodnota parametru vyhledávání @ref Configuration::efSearch "efSearch".
	 */
	constexpr uint DEFAULT_EF_SEARCH = 10;

	/**
	 * Konfigurace třídy @ref Index.
	 */
	class Configuration {
		/**
		 * Počet prvků, ze kterých index vybírá výsledky při zpracování dotazu.
		 */
		uint efSearch;

	public:
		/**
		 * Počet prvků, ze kterých index vybírá nové sousedy při vkládání nového prvku.
		 */
		const uint efConstruction;
		/**
		 * Maximální počet sousedů prvku na vrstvách vyšších než vrstva 0.
		 */
		const uint mMax;
		/**
		 * Maximální počet sousedů prvku na vrstvě 0.
		 * Dvojnásobek @ref mMax.
		 */
		const uint mMax0;

		/**
		 * Konstruktor.
		 * @param[in] efConstruction @ref efConstruction
		 * @param[in] mMax @ref mMax
		 */
		Configuration(const uint efConstruction, const uint mMax);
		/**
		 * Vrátí hodnotu @ref efSearch.
		 * @return @ref efSearch
		 */
		uint getEfSearch() const;
		/**
		 * Vrátí vyšší hodnotu při výběru mezi @ref efSearch a @p k.
		 * @param[in] k Počet hledaných sousedů pro dotazovaný prvek.
		 * @return max(@ref efSearch, @p k)
		 */
		uint getMaxEf(const uint k) const;
		/**
		 * Vrátí optimální hodnotu parametru stavby @ref LevelGenerator::mL "mL".
		 * @return 1 / log(@ref mMax)
		 */
		double getML() const;
		/**
		 * Nastaví hodnotu @ref efSearch.
		 * @param[in] efSearch @ref efSearch
		 */
		void setEfSearch(const uint efSearch);
	};
}
