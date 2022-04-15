#pragma once
#include <string>

#if defined(SIMD_CAPABLE)
	#include <immintrin.h>

	#ifdef _MSC_VER
		#include <intrin.h>
		#include <stdexcept>
	#else
		#include <x86intrin.h>
	#endif

	#if defined(__GNUC__)
		#define PORTABLE_ALIGN32 __attribute__((aligned(32)))
		#define PORTABLE_ALIGN64 __attribute__((aligned(64)))
	#else
		#define PORTABLE_ALIGN32 __declspec(align(32))
		#define PORTABLE_ALIGN64 __declspec(align(64))
	#endif
#endif

namespace chm {
	/**
	 * Metrika vzdálenosti.
	 */
	typedef float (*DistanceFunction)(
		const float*, const float*, const size_t,
		const size_t, const size_t, const size_t
	);

	/**
	 * Druh SIMD instrukcí.
	 */
	enum class SIMDType {
		AVX,
		AVX512,
		BEST,
		NONE,
		SSE
	};

	/**
	 * Získá nejlepší SIMDType dle počtu dvojic čísel,
	 * které zpracuje najednou.
	 */
	SIMDType getBestSIMDType();
	/**
	 * Získá SIMDType z jeho názvu.
	 */
	SIMDType getSIMDType(std::string s);

	/**
	 * Informace o metrice vzdálenosti.
	 */
	struct FunctionInfo {
		/**
		 * Ukazatel na metriku.
		 */
		const DistanceFunction f;
		/**
		 * Název metriky.
		 */
		const char* const name;

		/**
		 * Konstruktor.
		 */
		FunctionInfo(const DistanceFunction f, const char* const name);
	};

	/**
	 * Informace o metrice vzdálenosti a prostoru.
	 */
	struct DistanceInfo {
		/**
		 * Zbytek po dělení počtu dimenzí počtem složek zpracovaných
		 * během jedné iterace smyčky uvnitř metriky.
		 */
		const size_t dimLeft;
		/**
		 * Informace o metrice.
		 */
		const FunctionInfo funcInfo;

		/**
		 * Konstruktor.
		 */
		DistanceInfo(const size_t dimLeft, const FunctionInfo funcInfo);
	};
}
