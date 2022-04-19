#include <cmath>
#include "LevelGenerator.hpp"

namespace chm {
	uint LevelGenerator::getNext() {
		return uint(-std::log(this->dist(this->gen)) * this->mL);
	}

	LevelGenerator::LevelGenerator(const double mL, const uint seed) : dist(0.0, 1.0), gen(seed), mL(mL) {}
}
