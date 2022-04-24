#pragma once
#include <string>
#include <vector>

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
	 * Metrika vzdálenosti mezi dvěma prvky.
	 * @param[in] node Vektor prvního prvku.
	 * @param[in] query Vektor druhého prvku.
	 * @param[in] dim Počet dimenzí prostoru.
	 * @param[in] dim4 Počet dimenzí prostoru po dělení 4.
	 * @param[in] dim16 Počet dimenzí prostoru po dělení 16.
	 * @param[in] dimLeft Zbytek po dělení počtu dimenzí počtem složek vektoru
	 * zpracovaných během jedné iterace smyčky uvnitř metriky.
	 * @return Vzdálenost mezi dvěma prvky.
	 */
	typedef float (*DistanceFunction)(
		const float* node, const float* query, const size_t dim,
		const size_t dim4, const size_t dim16, const size_t dimLeft
	);

	/**
	 * Druh SIMD rozšíření instrukční sady procesoru.
	 */
	enum class SIMDType {
		/**
		 * Využít AVX nebo AVX2.
		 */
		AVX,
		/**
		 * Využít AVX-512.
		 */
		AVX512,
		/**
		 * Využít nejlepší dostupný druh SIMD rozšíření
		 * dle počtu zpracovaných dvojic čísel jednou instrukcí.
		 */
		BEST,
		/**
		 * Nevyužívat žádné SIMD rozšíření.
		 */
		NONE,
		/**
		 * Využít SSE.
		 */
		SSE
	};

	/**
	 * Vrátí všechny dostupné SIMD rozšíření.
	 * @return Všechny dostupné SIMD rozšíření.
	 * @see @ref SIMDType
	 */
	std::vector<SIMDType> getAvailableSIMD();
	/**
	 * Získá nejlepší @ref SIMDType dle počtu dvojic čísel, které zpracuje jedna instrukce.
	 * @return Nejlepší @ref SIMDType.
	 */
	SIMDType getBestSIMDType();
	/**
	 * Získá @ref SIMDType z jeho názvu.
	 * @param[in] s Název SIMD rozšíření.
	 * @return @ref SIMDType odpovídající názvu.
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
		 * Název funkce metriky.
		 */
		const char* const name;

		/**
		 * Konstruktor.
		 * @param[in] f @ref f
		 * @param[in] name @ref name
		 */
		FunctionInfo(const DistanceFunction f, const char* const name);
	};

	/**
	 * Informace o metrice vzdálenosti a prostoru.
	 */
	struct DistanceInfo {
		/**
		 * Zbytek po dělení počtu dimenzí počtem složek vektoru zpracovaných
		 * během jedné iterace smyčky uvnitř metriky.
		 */
		const size_t dimLeft;
		/**
		 * Informace o metrice.
		 */
		const FunctionInfo funcInfo;

		/**
		 * Konstruktor.
		 * @param[in] dimLeft @ref dimLeft
		 * @param[in] funcInfo @ref funcInfo
		 */
		DistanceInfo(const size_t dimLeft, const FunctionInfo funcInfo);
	};
}
