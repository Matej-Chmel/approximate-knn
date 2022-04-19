#pragma once
#include <type_traits>
#include "DistanceFunction.hpp"
#include "Neighbors.hpp"

namespace chm {
	/**
	 * Výsledek pokusu o navštívení prvního nenavštíveného prvku ze seznamu sousedů.
	 */
	struct VisitResult {
		/**
		 * Pozice prvního nenavštíveného prvku v seznamu sousedů.
		 */
		size_t idx;
		/**
		 * Identita prvního nenavštíveného prvku.
		 */
		uint neighborID;
		/**
		 * Pravda, pokud byl nalezen nenavštívený prvek.
		 */
		bool success;

		/**
		 * Vrátí objekt, který reprezentuje neúspěšný pokus o navštívení.
		 * @return Objekt reprezentující neúspěšný pokus o navštívení.
		 */
		static VisitResult fail();
		/**
		 * Konstruktor.
		 * @param[in] idx @ref idx
		 * @param[in] neighborID @ref neighborID
		 * @param[in] success @ref success
		 */
		VisitResult(const size_t idx, const uint neighborID, const bool success);
	};

	/**
	 * Seznam navštívených prvků.
	 * @tparam T Určuje typ informace o návštěvě prvku.
	 */
	template<typename T>
	class VisitedSet {
		/**
		 * Pole informací o návštěvě prvků.
		 */
		std::vector<T> v;

		/**
		 * Vrátí pravdu, pokud je @ref v bitovým polem.
		 * @return Pravda, pokud je @ref v bitovým polem.
		 */
		static constexpr bool isBitArray();

	public:
		/**
		 * Pokusí se navštívit daný prvek.
		 * @param[in] id Identita prvku.
		 * @return Pravda, pokud byl prvek před zavoláním této metody nenavštíven.
		 */
		bool insert(const uint id);
		/**
		 * Pokusí se navštívit první nenavštívený prvek ze seznamu sousedů.
		 * @param[in] N Seznam sousedů.
		 * @param[in] startIdx Pozice prvního prvku v seznamu sousedů, který dosud nebyl zkontrolován.
		 * @return Objekt reprezentující pokus o navštívení prvního nenavštíveného prvku ze seznamu sousedů.
		 */
		VisitResult insertNext(const Neighbors& N, const size_t startIdx);
		/**
		 * Asynchronně načte informace o návštěvách prvků do paměti.
		 * @param[in] id Pozice v poli @ref v, odkud budou data načteny.
		 */
		void prefetch(const uint id) const;
		/**
		 * Vynuluje seznam navštívených prvků a navštíví počáteční prvek.
		 * @param[in] count Počet prvků v indexu, které mohou být navštíveny.
		 * @param[in] epID Identita počátečního prvku.
		 */
		void prepare(const uint count, const uint epID);
		/**
		 * Konstruktor.
		 * @param[in] maxCount Maximální počet prvků v indexu.
		 */
		VisitedSet(const uint maxCount);
	};

	template<typename T>
	inline constexpr bool VisitedSet<T>::isBitArray() {
		return std::is_same<T, bool>::value;
	}

	template<typename T>
	inline bool VisitedSet<T>::insert(const uint id) {
		if(this->v[id])
			return false;

		this->v[id] = true;
		return true;
	}

	template<typename T>
	inline VisitResult VisitedSet<T>::insertNext(const Neighbors& N, const size_t startIdx) {
		const size_t len(N.len());

		if constexpr(VisitedSet<T>::isBitArray())
			for(size_t idx = startIdx; idx < len; idx++) {
				const auto id = N.get(idx);

				if(this->insert(id))
					return VisitResult(idx, id, true);
			}
		else {
			const auto lastIdx = len - 1;

			for(size_t idx = startIdx; idx < lastIdx; idx++) {
				this->prefetch(N.get(idx + 1));
				const auto id = N.get(idx);

				if(this->insert(id))
					return VisitResult(idx, id, true);
			}

			const auto id = N.get(lastIdx);

			if(this->insert(id))
				return VisitResult(lastIdx, id, true);
		}

		return VisitResult::fail();
	}

	template<typename T>
	inline void VisitedSet<T>::prefetch(const uint id) const {
		#if defined(SIMD_CAPABLE)
			if constexpr(!VisitedSet<T>::isBitArray())
				_mm_prefetch(reinterpret_cast<const char*>(this->v.data() + size_t(id)), _MM_HINT_T0);
		#endif
	}

	template<typename T>
	inline void VisitedSet<T>::prepare(const uint count, const uint epID) {
		std::fill(this->v.begin(), this->v.begin() + count, false);
		this->v[epID] = true;
	}

	template<typename T>
	inline VisitedSet<T>::VisitedSet(const uint maxCount) : v(maxCount, false) {}
}
