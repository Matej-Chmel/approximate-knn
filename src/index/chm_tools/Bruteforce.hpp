#pragma once
#include "chm/Node.hpp"
#include "chm/Space.hpp"

namespace chm {
	/**
	 * Třída pro vyhledávání nejbližších sousedů hrubou silou.
	 */
	class Bruteforce {
		/**
		 * Počet hledaných sousedů pro každý dotaz.
		 */
		const uint k;
		/**
		 * Pole prvků, které tento objekt třídí.
		 */
		std::vector<Node> nodes;
		/**
		 * Objekt, který ukládá vektory prvků a metriku vzdálenosti.
		 */
		Space space;

		/**
		 * Zapíše nejbližší sousedy jednoho dotazovaného prvku.
		 * @param[in] v Pole vektorů dotazovaných prvků.
		 * @param[out] neighbors Pole, kde metoda zapíše nejbližší sousedy.
		 * @param[in] queryIdx Pořadí dotazu.
		 */
		void queryOne(const std::vector<float>& v, std::vector<uint>& neighbors, const size_t queryIdx);

	public:
		/**
		 * Konstruktor.
		 * @param[in] dim Počet dimenzí prostoru.
		 * @param[in] k Počet hledaných sousedů pro každý dotaz.
		 * @param[in] maxCount Maximální počet prvků, ze kterých bude objekt vybírat sousedy.
		 * @param[in] simdType SIMD rozšíření, které se má použít.
		 * @param[in] spaceKind Druh prostoru dle metriky.
		 */
		Bruteforce(
			const size_t dim, const uint k, const size_t maxCount,
			const SIMDType simdType, const SpaceKind spaceKind
		);
		/**
		 * Vloží prvky z datové kolekce do objektu.
		 * @param[in] v Datová kolekce.
		 */
		void push(const std::vector<float>& v);
		/**
		 * Zpracuje kolekci dotazů.
		 * @param[in] v Kolekce dotazů.
		 * @param[out] neighbors Pole,
		 * kde metoda zapíše nejbližší sousedy pro všechny dotazy.
		 */
		void query(const std::vector<float>& v, std::vector<uint>& neighbors);
	};

	/**
	 * Funkční objekt, který srovnává prvky od
	 * nejmenší vzdálenosti ke zkoumanému prvku po největší.
	 */
	struct NodeCmp {
		/**
		 * Funkce objektu.
		 * @param[in] a První prvek.
		 * @param[in] b Druhý prvek.
		 * @return Pravda, pokud je vzdálenost prvku @p a od zkoumaného prvku menší
		 * než vzdálenost prvku @p b od zkoumaného prvku.
		 */
		constexpr bool operator()(const Node& a, const Node& b) const noexcept {
			return a.distance < b.distance;
		}
	};
}
