#pragma once
#include "Neighbors.hpp"

namespace chm {
	class Connections {
		std::vector<uint> layer0;
		const uint maxLen;
		const uint maxLen0;
		std::vector<std::vector<uint>> upperLayers;

	public:
		Connections(const uint maxNodeCount, const uint mMax, const uint mMax0);
		Neighbors getNeighbors(const uint id, const uint lc);
		void init(const uint id, const uint level);
	};
}
