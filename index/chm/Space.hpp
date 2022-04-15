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
		 * Prostor využívá metriky eukleidovské vzdálenosti.
		 */
		EUCLIDEAN,
		/**
		 * Prostor využívá skalárního součtu pro určení vzdálenosti.
		 */
		INNER_PRODUCT
	};

	/**
	 * Reprezentuje prostor. Ukládá vektory prvků a metriku vzdálenosti.
	 */
	class Space {
		/**
		 * Počet vložených prvků.
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
		 */
		float getNorm(const float* const data) const;
		/**
		 * Zapíše jednotkový vektor do pole res.
		 */
		void normalizeData(const float* const data, float* const res) const;

	public:
		/**
		 * Počet dimenzí prostoru.
		 */
		const size_t dim;

		/**
		 * Vrátí count.
		 */
		uint getCount() const;
		/**
		 * Vrátí ukazatel na vektor prvku s identitou id.
		 */
		const float* const getData(const uint id) const;
		/**
		 * Vrátí vzdálenost mezi dvěma prvky.
		 */
		float getDistance(const uint aID, const uint bID) const;
		/**
		 * Vrátí vzdálenost mezi dvěma vektory.
		 */
		float getDistance(const float* const a, const float* const b) const;
		/**
		 * Vrátí vzdálenost mezi vektorem a prvkem.
		 */
		float getDistance(const float* const aData, const uint bID) const;
		/**
		 * Vrátí název funkce metriky vzdálenost.
		 */
		std::string getDistanceName() const;
		/**
		 * Pokud je normalized pravda, vrátí jednotkový vektor dotazu, jinak vrátí původní vektor dotazu.
		 */
		const float* const getNormalizedQuery(const float* const data);
		/**
		 * Vrátí pravdu, pokud dosud nebyl vložen žádný prvek do prostoru.
		 */
		bool isEmpty() const;
		/**
		 * Asynchronně načte vektor prvku s identitou id do paměti.
		 */
		void prefetch(const uint id) const;
		/**
		 * Vloží datovou kolekci do prostoru.
		 */
		void push(const float* const data, const uint count);
		/**
		 * Konstruktor.
		 */
		Space(const size_t dim, const SpaceKind kind, const size_t maxCount, const SIMDType simdType);
	};
}
