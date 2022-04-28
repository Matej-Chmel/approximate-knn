#include "Dataset.hpp"

namespace chm {
	void generateRandomData(
		std::vector<float>& v, const size_t count, const size_t dim, const uint seed
	) {
		const auto components = count * dim;
		std::uniform_real_distribution<float> dist{};
		std::default_random_engine gen(seed);

		for(size_t i = 0; i < components; i++)
			v[i] = dist(gen);
	}

	void throwCouldNotOpen(const fs::path& p) {
		std::stringstream s;
		s << "Could not open file " << p << '.';
		throw std::runtime_error(s.str());
	}
}
