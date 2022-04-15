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
		 */
		template<typename T>
		const T* const getNumpyPtr(const NumpyArray<T>& a) {
			return (const T* const)a.request().ptr;
		}

		/**
		 * Vrátí počet vektorů v NumPy poli.
		 */
		template<typename T>
		size_t getNumpyXDim(const NumpyArray<T>& a) {
			return (size_t)a.request().shape[0];
		}

		/**
		 * Vrátí počet složek jednoho vektoru v NumPy poli.
		 */
		template<typename T>
		size_t getNumpyYDim(const NumpyArray<T>& a) {
			return (size_t)a.request().shape[1];
		}
	#endif

	/**
	 * Obal pro obyčejné i NumPy pole.
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
		 */
		FloatArray(const float* const data, const uint count);

		#ifdef PYBIND_INCLUDED
			/**
			 * Konstruktor z NumPy pole.
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
		 * Pravda, pokud tento objekt vlastní pole distances a labels.
		 */
		bool owningData;

	public:
		/**
		 * Destruktor.
		 */
		~KnnResults();
		/**
		 * Vrátí odkaz na pole identit výsledných sousedů, přes který je nelze změnit.
		 */
		const uint* const getLabels() const;
		KnnResults(const KnnResults& o) = delete;
		/**
		 * Konstruktor přemístění výsledků z jiného objektu.
		 */
		KnnResults(KnnResults&& o) noexcept;
		/**
		 * Konstruktor.
		 */
		KnnResults(const size_t count, const size_t k);
		KnnResults& operator=(const KnnResults&) = delete;
		KnnResults& operator=(KnnResults&& o) noexcept = delete;
		/**
		 * Nastaví identitu a vzdálenost jednoho souseda.
		 */
		void setData(const size_t queryIdx, const size_t neighborIdx, const float distance, const uint label);

		#ifdef PYBIND_INCLUDED
			/**
			 * Vytvoří uspořádanou dvojici (labels, distances) a vrátí ji interpretu jazyka Python.
			 */
			py::tuple makeTuple();
		#endif
	};
}
