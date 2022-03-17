#include "DistanceInfo.hpp"
#include "enhancedInstructionSet.hpp"

namespace chm {
	std::string getArchName() {
		#if defined(USE_AVX)
			return "AVX";
		#elif defined(USE_AVX512)
			return "AVX512";
		#elif defined(USE_SSE)
			return "SSE";
		#else
			return "";
		#endif
	}

	DistanceInfo::DistanceInfo(const size_t dimLeft, const DistanceFunction distFunc, const std::string name)
		: dimLeft(dimLeft), distFunc(distFunc), name(name + getArchName()) {}
}
