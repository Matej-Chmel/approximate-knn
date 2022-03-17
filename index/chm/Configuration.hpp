#pragma once
#include "types.hpp"

namespace chm {
	constexpr uint DEFAULT_EF_SEARCH = 10;

	class Configuration {
		uint efSearch;

	public:
		const uint efConstruction;
		const uint mMax;
		const uint mMax0;

		Configuration(const uint efConstruction, const uint mMax);
		uint getEfSearch() const;
		uint getMaxEf(const uint k) const;
		double getML() const;
		void setEfSearch(const uint efSearch);
	};
}
