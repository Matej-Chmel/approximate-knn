#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "Configuration.hpp"

namespace chm {
	Configuration::Configuration(const uint efConstruction, const uint mMax)
		: efSearch(DEFAULT_EF_SEARCH), efConstruction(efConstruction),
		mMax(mMax), mMax0(this->mMax * 2) {

			if(!this->efConstruction)
				throw std::invalid_argument(
					"Parameter \"efConstruction\" must be greater than 0."
				);
			if(!this->mMax)
				throw std::invalid_argument(
					"Parameter \"mMax\" must be greater than 0."
				);
		}

	uint Configuration::getEfSearch() const {
		return this->efSearch;
	}

	uint Configuration::getMaxEf(const uint k) const {
		return std::max(this->efSearch, k);
	}

	double Configuration::getML() const {
		return 1.0 / std::log(double(this->mMax));
	}

	void Configuration::setEfSearch(const uint efSearch) {
		this->efSearch = efSearch;
	}
}
