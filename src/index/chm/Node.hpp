#pragma once
#include "types.hpp"

namespace chm {
	/**
	 * Vrchol haldy.
	 */
	struct Node {
		/**
		 * Vzdálenost vrcholu od zkoumaného prvku.
		 */
		float distance;
		/**
		 * Identita prvku, který je zastupován vrcholem.
		 */
		uint id;

		/**
		 * Konstruktor s nulovými hodnotami datových polí @ref distance a @ref id.
		 */
		Node();
		/**
		 * Konstruktor.
		 * @param[in] distance @ref distance
		 * @param[in] id @ref id
		 */
		Node(const float distance, const uint id);
	};

	/**
	 * Funkční objekt, který zajistí,
	 * že kořenem haldy je vrchol s největší vzdáleností od zkoumaného prvku.
	 */
	struct FarComparator {
		/**
		 * Funkce objektu.
		 * @param[in] a První vrchol.
		 * @param[in] b Druhý vrchol.
		 * @return Pravda, pokud je vzdálenost vrcholu @p a od zkoumaného prvku menší
		 * než vzdálenost vrcholu @p b od zkoumaného prvku.
		 */
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance < b.distance;
		}
	};

	/**
	 * Funkční objekt, který zajistí,
	 * že kořenem haldy je vrchol s nejmenší vzdáleností od zkoumaného prvku.
	 */
	struct NearComparator {
		/**
		 * Funkce objektu.
		 * @param[in] a První vrchol.
		 * @param[in] b Druhý vrchol.
		 * @return Pravda, pokud je vzdálenost vrcholu @p a od zkoumaného prvku větší
		 * než vzdálenost vrcholu @p b od zkoumaného prvku.
		 */
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance > b.distance;
		}
	};
}
