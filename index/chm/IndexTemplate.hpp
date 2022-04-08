#pragma once
#include "VisitedSet.hpp"

namespace chm {
	enum class IndexTemplate {
		HEURISTIC,
		NAIVE,
		NO_BIT_ARRAY,
		PREFETCHING
	};

	IndexTemplate getIndexTemplate(std::string s);

	struct HeuristicTemplate {
		static constexpr bool useHeuristic = true;
		static constexpr bool usePrefetch = false;
		using VisitedSet = chm::VisitedSet<bool>;
	};

	struct NaiveTemplate {
		static constexpr bool useHeuristic = false;
		static constexpr bool usePrefetch = false;
		using VisitedSet = chm::VisitedSet<bool>;
	};

	struct NoBitArrayTemplate {
		static constexpr bool useHeuristic = true;
		static constexpr bool usePrefetch = true;
		using VisitedSet = chm::VisitedSet<unsigned char>;
	};

	struct PrefetchingTemplate {
		static constexpr bool useHeuristic = true;
		static constexpr bool usePrefetch = true;
		using VisitedSet = chm::VisitedSet<bool>;
	};
}
