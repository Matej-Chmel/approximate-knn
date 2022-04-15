#pragma once
#include <unordered_set>
#include "KnnResults.hpp"

namespace chm {
	/**
	 * Obal pole identit výsledných sousedů ze zpracování kolekce dotazů.
	 */
	class LabelsWrapper {
		/**
		 * Ukazatel na pole identit.
		 */
		const uint* const data;

	public:
		/**
		 * Počet dotazů.
		 */
		const size_t xDim;
		/**
		 * Počet sousedů každého dotazu.
		 */
		const size_t yDim;

		/**
		 * Vloží identity sousedů pro dotaz x do množiny set.
		 */
		void fillSet(std::unordered_set<uint>& set, const size_t x) const;
		/**
		 * Vrátí identitu souseda na pozici y ve výsledku dotazu x.
		 */
		uint get(const size_t x, const size_t y) const;
		/**
		 * Vrátí celkový počet uložených identit, což je xDim * yDim.
		 */
		size_t getComponentCount() const;
		/**
		 * Konstruktor z obyčejného pole.
		 */
		LabelsWrapper(const uint* const data, const size_t xDim, const size_t yDim);

		#ifdef PYBIND_INCLUDED
			/**
			 * Konstruktor z NumPy pole.
			 */
			LabelsWrapper(const NumpyArray<uint>& a);
		#endif
	};

	/**
	 * Vrátí přesnost výsledků reprezentovaných obyčejným polem.
	 */
	float getRecall(const uint* const correctLabels, const uint* const testedLabels, const size_t queryCount, const size_t k);
	/**
	 * Vrátí přesnost výsledků reprezentovaných objektem třídy LabelsWrapper.
	 */
	float getRecall(const LabelsWrapper& correctLabels, const LabelsWrapper& testedLabels);

	#ifdef PYBIND_INCLUDED
		/**
		 * Vrátí přesnost výsledků reprezentovaných NumPy polem.
		 */
		float getRecall(const NumpyArray<uint> correctLabels, const NumpyArray<uint> testedLabels);
	#endif
}
