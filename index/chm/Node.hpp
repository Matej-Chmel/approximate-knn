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
		 * Identita prvku, který vrchol reprezentuje.
		 */
		uint id;

		/**
		 * Konstruktor s nulovými hodnotami datových polí.
		 */
		Node();
		/**
		 * Konstruktor.
		 */
		Node(const float distance, const uint id);
	};

	/**
	 * Funkce, která zajistí, že kořenem haldy je vrchol s největší vzdáleností.
	 */
	struct FarComparator {
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance < b.distance;
		}
	};

	/**
	 * Funkce, která zajistí, že kořenem haldy je vrchol s nejmenší vzdáleností.
	 */
	struct NearComparator {
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance > b.distance;
		}
	};
}
