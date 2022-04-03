#include <algorithm>
#include <cctype>
#include <stdexcept>
#include "DistanceFunction.hpp"

namespace chm {
	SIMDType getSIMDType(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), std::tolower);

		if(s == "avx")
			return SIMDType::AVX;
		if(s == "avx512")
			return SIMDType::AVX512;
		if(s == "none")
			return SIMDType::NONE;
		if(s == "sse")
			return SIMDType::SSE;

		throw std::runtime_error("Invalid SIMD type.");
	}

	FunctionInfo(const DistanceFunction f, const char* const name) : f(f), name(name) {}
	DistanceInfo(const size_t dimLeft, const FunctionInfo funcInfo)
		: dimLeft(dimLeft), funcInfo(funcInfo) {}
}
