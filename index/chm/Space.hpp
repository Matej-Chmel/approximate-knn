#pragma once
#include <vector>
#include "DistanceFunction.hpp"
#include "types.hpp"

namespace chm {
	/**
	 * Výčet druhů prostoru dle metriky.
	 */
	enum class SpaceKind {
		/**
		 * Prostor využívá metriky kosinusové vzdálenosti.
		 */
		ANGULAR,
		/**
		 * Prostor využívá metriky Eukleidovské vzdálenosti.
		 */
		EUCLIDEAN,
		/**
		 * Prostor využívá skalárního součinu pro určení vzdálenosti.
		 */
		INNER_PRODUCT
	};

	/**
	 * Reprezentuje prostor. Ukládá vektory prvků a metriku vzdálenosti.
	 */
	class Space {
		/**
		 * Počet prvků v prostoru.
		 */
		uint count;
		/**
		 * Pole vektorů prvků.
		 */
		std::vector<float> data;
		/**
		 * Počet dimenzí prostoru po dělení 4.
		 */
		const size_t dim4;
		/**
		 * Počet dimenzí prostoru po dělení 16.
		 */
		const size_t dim16;
		/**
		 * Metrika vzdálenosti.
		 */
		const DistanceInfo distInfo;
		/**
		 * Pravda, pokud objekt ukládá jednotkové vektory prvků.
		 */
		const bool normalize;
		/**
		 * Vektor dotazovaného prvku, který index právě zpracovává.
		 */
		std::vector<float> query;

		/**
		 * Vrátí délku vektoru.
		 * @param[in] data Vektor.
		 * @return Délka vektoru.
		 */
		float getNorm(const float* const data) const;
		/**
		 * Vypočítá jednotkový vektor.
		 * @param[in] data Původní vektor.
		 * @param[out] res Výsledný jednotkový vektor.
		 */
		void normalizeData(const float* const data, float* const res) const;

	public:
		/**
		 * Počet dimenzí prostoru.
		 */
		const size_t dim;

		/**
		 * Vrátí @ref count.
		 * @return @ref count.
		 */
		uint getCount() const;
		/**
		 * Vrátí ukazatel na vektor prvku.
		 * @param[in] id Identita prvku.
		 * @return Ukazatel na vektor prvku.
		 */
		const float* const getData(const uint id) const;
		/**
		 * Vrátí vzdálenost mezi dvěma prvky.
		 * @param[in] aID Identita prvního prvku.
		 * @param[in] bID Identita druhého prvku.
		 * @return Vzdálenost.
		 */
		float getDistance(const uint aID, const uint bID) const;
		/**
		 * Vrátí vzdálenost mezi dvěma vektory.
		 * @param[in] a První vektor.
		 * @param[in] b Druhý vektor.
		 * @return Vzdálenost.
		 */
		float getDistance(const float* const a, const float* const b) const;
		/**
		 * Vrátí vzdálenost mezi vektorem a prvkem.
		 * @param[in] aData Vektor.
		 * @param[in] bID Identita prvku.
		 * @return Vzdálenost.
		 */
		float getDistance(const float* const aData, const uint bID) const;
		/**
		 * Vrátí název funkce metriky vzdálenosti.
		 * @return @ref distInfo::funcInfo::name.
		 */
		std::string getDistanceName() const;
		/**
		 * Pokud je @ref normalize pravda, vrátí jednotkový vektor dotazu, jinak vrátí původní vektor dotazu.
		 * @param[in] data Původní vektor dotazu.
		 * @return Ukazatel na výsledný vektor.
		 */
		const float* const getNormalizedQuery(const float* const data);
		/**
		 * Vrátí pravdu, pokud dosud nebyl vložen žádný prvek do prostoru.
		 * @return Pravda, pokud je prostor prázdný.
		 */
		bool isEmpty() const;
		/**
		 * Asynchronně načte vektor prvku do paměti.
		 * @param[in] id Identita prvku.
		 */
		void prefetch(const uint id) const;
		/**
		 * Vloží kolekci prvků do prostoru.
		 * @param[in] data Ukazatel na vektor prvního prvku v kolekci.
		 * @param[in] count Počet prvků v kolekci.
		 */
		void push(const float* const data, const uint count);
		/**
		 * Konstruktor.
		 * @param[in] dim @ref dim
		 * @param[in] kind Druh prostoru dle metriky.
		 * @param[in] maxCount Maximální počet prvků v prostoru.
		 * @param[in] simdType Druh SIMD instrukcí využívaných při výpočtu vzdálenosti mezi prvky.
		 */
		Space(const size_t dim, const SpaceKind kind, const size_t maxCount, const SIMDType simdType);
	};
}
