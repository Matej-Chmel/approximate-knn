#include "Dataset.hpp"

namespace chm {
	void throwCouldNotOpen(const fs::path& p) {
		std::stringstream s;
		s << "Could not open file " << p << '.';
		throw std::runtime_error(s.str());
	}
}
