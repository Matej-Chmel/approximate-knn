#include "Connections.hpp"

namespace chm {
	Connections::Connections(const uint maxNodeCount, const uint mMax, const uint mMax0)
		: maxLen(mMax + 1), maxLen0(mMax0 + 1) {

		this->layer0.resize(maxNodeCount * this->maxLen0, 0);
		this->upperLayers.resize(maxNodeCount);
	}

	Neighbors Connections::getNeighbors(const uint id, const uint lc) {
		return Neighbors(
			lc
			? this->upperLayers[id].begin() + this->maxLen * (lc - 1)
			: this->layer0.begin() + this->maxLen0 * id
		);
	}

	void Connections::init(const uint id, const uint level) {
		if(level)
			this->upperLayers[id].resize(this->maxLen * level, 0);
	}
}
