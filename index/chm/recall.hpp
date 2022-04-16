#pragma once
#include <unordered_set>
#include "KnnResults.hpp"

namespace chm {
	/**
	 * Obal pole identit výsledných sousedů ze zpracování kolekce dotazů.
	 */
	class LabelsWrapper {
		/**
		 * Pole identit.
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
		 * Vloží identity sousedů pro daný dotaz do množiny unikátních identit.
		 * @param[in] set Množina unikátních identit.
		 * @param[in] x Pozice dotazu v poli @ref data.
		 */
		void fillSet(std::unordered_set<uint>& set, const size_t x) const;
		/**
		 * Vrátí identitu souseda na pozici @p y ve výsledku dotazu @p x.
		 * @param[in] x Pozice dotazu v poli @ref data.
		 * @param[in] y Pozice souseda vůči pozici dotazu.
		 * @return Identita souseda na pozici @p y ve výsledku dotazu @p x.
		 */
		uint get(const size_t x, const size_t y) const;
		/**
		 * Vrátí celkový počet uložených identit.
		 * @return @ref xDim * @ref yDim
		 */
		size_t getComponentCount() const;
		/**
		 * Konstruktor z obyčejného pole.
		 * @param[in] data @ref data
		 * @param[in] xDim @ref xDim
		 * @param[in] yDim @ref yDim
		 */
		LabelsWrapper(const uint* const data, const size_t xDim, const size_t yDim);

		#ifdef PYBIND_INCLUDED
			/**
			 * Konstruktor z NumPy pole.
			 * @param[in] a NumPy pole.
			 */
			LabelsWrapper(const NumpyArray<uint>& a);
		#endif
	};

	/**
	 * Vrátí přesnost výsledků zpracování kolekce dotazů reprezentovaných obyčejným polem.
	 * @param[in] correctLabels Výsledky exaktní metody KNNS.
	 * @param[in] testedLabels Výsledky testované metody ANNS.
	 * @param[in] queryCount Počet dotazů.
	 * @return Přesnost.
	 */
	float getRecall(const uint* const correctLabels, const uint* const testedLabels, const size_t queryCount, const size_t k);
	/**
	 * Vrátí přesnost výsledků zpracování kolekce dotazů reprezentovaných objektem třídy @ref LabelsWrapper.
	 * @param[in] correctLabels Výsledky exaktní metody KNNS.
	 * @param[in] testedLabels Výsledky testované metody ANNS.
	 * @return Přesnost.
	 */
	float getRecall(const LabelsWrapper& correctLabels, const LabelsWrapper& testedLabels);

	#ifdef PYBIND_INCLUDED
		/**
		 * Vrátí přesnost výsledků zpracování kolekce dotazů reprezentovaných NumPy polem.
		 * @param[in] correctLabels Výsledky exaktní metody KNNS.
		 * @param[in] testedLabels Výsledky testované metody ANNS.
		 * @return Přesnost.
		 */
		float getRecall(const NumpyArray<uint> correctLabels, const NumpyArray<uint> testedLabels);
	#endif
}
