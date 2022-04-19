#pragma once
#include "types.hpp"

#ifdef PYBIND_INCLUDED
	#include <pybind11/numpy.h>
	#include <pybind11/pybind11.h>
#endif

namespace chm {
	#ifdef PYBIND_INCLUDED
		namespace py = pybind11;

		/**
		 * Typ NumPy pole.
		 */
		template<typename T>
		using NumpyArray = py::array_t<T, py::array::c_style | py::array::forcecast>;

		/**
		 * Vrátí ukazatel na první vektor NumPy pole.
		 * @tparam T Typ čísel v NumPy poli.
		 * @param[in] a NumPy pole.
		 * @return Ukazatel na první vektor.
		 */
		template<typename T>
		const T* const getNumpyPtr(const NumpyArray<T>& a) {
			return (const T* const)a.request().ptr;
		}

		/**
		 * Vrátí počet vektorů v NumPy poli.
		 * @tparam T Typ čísel v NumPy poli.
		 * @param[in] a NumPy pole.
		 * @return Počet vektorů.
		 */
		template<typename T>
		size_t getNumpyXDim(const NumpyArray<T>& a) {
			return (size_t)a.request().shape[0];
		}

		/**
		 * Vrátí počet složek jednoho vektoru v NumPy poli.
		 * @tparam T Typ čísel v NumPy poli.
		 * @param[in] a NumPy pole.
		 * @return Počet složek jednoho vektoru.
		 */
		template<typename T>
		size_t getNumpyYDim(const NumpyArray<T>& a) {
			return (size_t)a.request().shape[1];
		}
	#endif

	/**
	 * Obal pro obyčejné i @ref NumpyArray "NumPy" pole.
	 */
	struct FloatArray {
		/**
		 * Počet vektorů v poli.
		 */
		const uint count;
		/**
		 * Ukazatel na první vektor.
		 */
		const float* const data;

		/**
		 * Konstruktor z obyčejného pole.
		 * @param[in] data @ref data
		 * @param[in] count @ref count
		 */
		FloatArray(const float* const data, const uint count);

		#ifdef PYBIND_INCLUDED
			/**
			 * Konstruktor z NumPy pole.
			 * @param[in] data NumPy pole.
			 * @param[in] dim Počet složek jednoho vektoru v NumPy poli.
			 */
			FloatArray(const NumpyArray<float>& data, const size_t dim);
		#endif
	};

	/**
	 * Výsledky zpracování kolekce dotazů.
	 */
	class KnnResults {
		/**
		 * Počet dotazů.
		 */
		const size_t count;
		/**
		 * Pole vzdáleností výsledných sousedů od dotazovaného prvku.
		 */
		float* const distances;
		/**
		 * Počet hledaných sousedů pro jeden dotaz.
		 */
		const size_t k;
		/**
		 * Pole identit výsledných sousedů.
		 */
		uint* const labels;
		/**
		 * Pravda, pokud tento objekt vlastní paměť polí @ref distances a @ref labels.
		 */
		bool owningData;

	public:
		/**
		 * Destruktor.
		 */
		~KnnResults();
		/**
		 * Vrátí odkaz na pole @ref labels, přes který nelze upravovat data tohoto pole.
		 * @return Odkaz na pole @ref labels.
		 */
		const uint* const getLabels() const;
		/**
		 * Výsledky nelze kopírovat pomocí konstruktoru.
		 */
		KnnResults(const KnnResults& o) = delete;
		/**
		 * Konstruktor přemístění výsledků z jiného objektu.
		 */
		KnnResults(KnnResults&& o) noexcept;
		/**
		 * Konstruktor.
		 * @param[in] count @ref count
		 * @param[in] k @ref k
		 */
		KnnResults(const size_t count, const size_t k);
		/**
		 * Výsledky nelze kopírovat pomocí operátoru přiřazení.
		 */
		KnnResults& operator=(const KnnResults&) = delete;
		/**
		 * Data výsledků nelze přemístit pomocí operátoru přiřazení.
		 */
		KnnResults& operator=(KnnResults&& o) noexcept = delete;
		/**
		 * Nastaví identitu a vzdálenost jednoho souseda od dotazovaného prvku.
		 * @param[in] queryIdx Pozice dotazu.
		 * @param[in] neighborIdx Pozice souseda vůči pozici dotazu.
		 * @param[in] distance Vzdálenost souseda od dotazovaného prvku.
		 * @param[in] label Identita souseda.
		 */
		void setData(const size_t queryIdx, const size_t neighborIdx, const float distance, const uint label);

		#ifdef PYBIND_INCLUDED
			/**
			 * Vytvoří uspořádanou dvojici (@ref labels, @ref distances) a vrátí ji interpretu jazyka Python.
			 * @return Uspořádaná dvojice (@ref labels, @ref distances).
			 */
			py::tuple makeTuple();
		#endif
	};
}
