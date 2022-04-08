#include <cctype>
#include <stdexcept>
#include "IndexTemplate.hpp"

namespace chm {
	IndexTemplate getIndexTemplate(std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);

		if(s == "heuristic")
			return IndexTemplate::HEURISTIC;
		else if(s == "naive")
			return IndexTemplate::NAIVE;
		else if(s.rfind("nobit") == 0)
			return IndexTemplate::NO_BIT_ARRAY;
		else if(s.rfind("prefetch") == 0)
			return IndexTemplate::PREFETCHING;
		throw std::runtime_error("Invalid index template.");
	}
}
