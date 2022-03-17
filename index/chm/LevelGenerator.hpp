#pragma once
#include <random>
#include "types.hpp"

namespace chm {
	class LevelGenerator {
		std::uniform_real_distribution<double> dist;
		std::default_random_engine gen;
		const double mL;

	public:
		uint getNext();
		LevelGenerator(const double mL, const uint seed);
	};
}
