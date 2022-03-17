#pragma once

#ifndef NO_MANUAL_VECTORIZATION
	#ifdef __SSE__
		#define USE_ENHANCED

		#ifdef __AVX__
			#ifdef __AVX512F__
				#define USE_AVX512
			#else
				#define USE_AVX
			#endif
		#else
			#define USE_SSE
		#endif
	#endif
#endif

#if defined(USE_AVX) || defined(USE_SSE)
	#ifdef _MSC_VER
		#include <intrin.h>
		#include <stdexcept>
	#else
		#include <x86intrin.h>
	#endif

	#if defined(USE_AVX512)
		#include <immintrin.h>
	#endif

	#if defined(__GNUC__)
		#define PORTABLE_ALIGN32 __attribute__((aligned(32)))
		#define PORTABLE_ALIGN64 __attribute__((aligned(64)))
	#else
		#define PORTABLE_ALIGN32 __declspec(align(32))
		#define PORTABLE_ALIGN64 __declspec(align(64))
	#endif
#endif
