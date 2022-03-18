#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Dataset.hpp"
#include "recall.hpp"

namespace chm {
	void Dataset::build(const IndexPtr& index) const {
		index->push(this->train.data(), this->trainCount);
	}

	Dataset::Dataset(const fs::path& p) {
		std::ifstream file(p, std::ios::binary);

		if (!file.is_open()) {
			std::stringstream s;
			s << "Could not open dataset file " << p << '.';
			throw std::runtime_error(s.str());
		}

		bool angular;
		uint dim;
		readBinary(file, angular);
		this->space = angular ? SpaceKind::ANGULAR : SpaceKind::EUCLIDEAN;
		readBinary(file, dim);
		this->dim = size_t(dim);
		readBinary(file, this->k);
		readBinary(file, this->testCount);
		readBinary(file, this->trainCount);
		readBinary(file, this->neighbors, size_t(this->k * this->testCount));
		readBinary(file, this->test, size_t(this->dim * this->testCount));
		readBinary(file, this->train, size_t(this->dim * this->trainCount));
	}

	IndexPtr Dataset::getIndex(const uint efConstruction, const uint mMax, const uint seed) const {
		return std::make_shared<Index>(this->dim, efConstruction, this->trainCount, mMax, seed, this->space);
	}

	float Dataset::getRecall(const KnnResults& results) const {
		return chm::getRecall(this->neighbors.data(), results.getLabels(), this->testCount, this->k);
	}

	KnnResults Dataset::query(const IndexPtr& index, const uint efSearch) const {
		index->setEfSearch(efSearch);
		return index->query(this->test.data(), this->testCount, this->k);
	}

	void Dataset::writeDescription(const fs::path& p) const {
		std::ofstream s(p);
		this->writeDescription(s);
	}

	void Dataset::writeDescription(std::ostream& s) const {
		s << "angular: " << (this->space == SpaceKind::ANGULAR ? "True" : "False") << '\n'
			<< "dim: " << dim << '\n'
			<< "k: " << k << '\n'
			<< "testCount: " << testCount << '\n'
			<< "trainCount: " << trainCount << '\n';
		chm::writeDescription(s, this->neighbors, "neighbors");
		chm::writeDescription(s, this->test, "test", 6);
		chm::writeDescription(s, this->train, "train", 6);
	}
}
