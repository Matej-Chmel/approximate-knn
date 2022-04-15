#pragma once
#include "VisitedSet.hpp"

namespace chm {
	/**
	 * Výčet šablon indexu.
	 */
	enum class IndexTemplate {
		/**
		* Šablona indexu využívající heuristiku.
		*/
		HEURISTIC,
		/**
		 * Šablona indexu využívající naivní algoritmus.
		 */
		NAIVE,
		/**
		 * Šablona indexu využívající obyčejného pole pro reprezentaci seznamu navštívených vrcholů.
		 */
		NO_BIT_ARRAY,
		/**
		 * Šablona indexu využívající asynchronního přístupu do paměti při výpočtu vzdálenosti.
		 */
		PREFETCHING
	};

	/**
	 * Získá šablonu indexu z jejího názvu.
	 */
	IndexTemplate getIndexTemplate(std::string s);

	/**
	 * Šablona indexu využívající heuristiku.
	 */
	struct HeuristicTemplate {
		/**
		 * Pro výběr sousedů šablona využívá heuristiku.
		 */
		static constexpr bool useHeuristic = true;
		/**
		 * Šablona nevyužívá asynchronního přístupu do paměti.
		 */
		static constexpr bool usePrefetch = false;
		/**
		 * Šablona využívá bitového pole pro reprezentaci seznamu navštívených vrcholů.
		 */
		using VisitedSet = chm::VisitedSet<bool>;
	};

	/**
	 * Šablona indexu využívající naivní algoritmus.
	 */
	struct NaiveTemplate {
		/**
		 * Pro výběr sousedů šablona využívá naivní algoritmus.
		 */
		static constexpr bool useHeuristic = false;
		/**
		 * Šablona nevyužívá asynchronního přístupu do paměti.
		 */
		static constexpr bool usePrefetch = false;
		/**
		 * Šablona využívá bitového pole pro reprezentaci seznamu navštívených vrcholů.
		 */
		using VisitedSet = chm::VisitedSet<bool>;
	};

	/**
	 * Šablona indexu využívající obyčejného pole pro reprezentaci seznamu navštívených vrcholů.
	 */
	struct NoBitArrayTemplate {
		/**
		 * Pro výběr sousedů šablona využívá heuristiku.
		 */
		static constexpr bool useHeuristic = true;
		/**
		 * Šablona využívá asynchronního přístupu do paměti při výpočtu vzdálenosti.
		 */
		static constexpr bool usePrefetch = true;
		/**
		 * Šablona využívá obyčejného pole pro reprezentaci seznamu navštívených vrcholů.
		 */
		using VisitedSet = chm::VisitedSet<unsigned char>;
	};

	/**
	 * Šablona indexu využívající asynchronního přístupu do paměti při výpočtu vzdálenosti.
	 */
	struct PrefetchingTemplate {
		/**
		 * Pro výběr sousedů šablona využívá heuristiku.
		 */
		static constexpr bool useHeuristic = true;
		/**
		 * Šablona využívá asynchronního přístupu do paměti při výpočtu vzdálenosti.
		 */
		static constexpr bool usePrefetch = true;
		/**
		 * Šablona využívá bitového pole pro reprezentaci seznamu navštívených vrcholů.
		 */
		using VisitedSet = chm::VisitedSet<bool>;
	};
}
