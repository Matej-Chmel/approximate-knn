#include <fstream>
#include <sstream>
#include <stdexcept>
#include "chm/recall.hpp"
#include "Dataset.hpp"

namespace chm {
	void Dataset::build(const IndexPtr& index) const {
		index->push(this->train.data(), this->trainCount);
	}

	Dataset::Dataset(const fs::path& p) : name(p.stem().string()) {
		std::ifstream file(p, std::ios::binary);

		if (!file.is_open())
			throwCouldNotOpen(p);

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

	IndexPtr Dataset::getIndex(const uint efConstruction, const uint mMax, const uint seed, const bool useHeuristic) const {
		return std::make_shared<Index>(this->dim, efConstruction, this->trainCount, mMax, seed, this->space, useHeuristic);
	}

	float Dataset::getRecall(const KnnResults& results) const {
		return chm::getRecall(this->neighbors.data(), results.getLabels(), this->testCount, this->k);
	}

	bool Dataset::isAngular() const {
		switch(this->space) {
			case SpaceKind::ANGULAR:
				return true;
			case SpaceKind::EUCLIDEAN:
				return false;
			default:
				throw std::runtime_error("Invalid space.");
		}
	}

	KnnResults Dataset::query(const IndexPtr& index, const uint efSearch) const {
		index->setEfSearch(efSearch);
		return index->query(this->test.data(), this->testCount, this->k);
	}

	void Dataset::writeLongDescription(const fs::path& p) const {
		std::ofstream s(p);
		this->writeLongDescription(s);
	}

	void Dataset::writeLongDescription(std::ostream& s) const {
		s << "angular: " << (this->isAngular() ? "True" : "False") << '\n'
			<< "dim: " << dim << '\n'
			<< "k: " << k << '\n'
			<< "testCount: " << testCount << '\n'
			<< "trainCount: " << trainCount << '\n';
		chm::writeDescription(s, this->neighbors, "neighbors");
		chm::writeDescription(s, this->test, "test", 6);
		chm::writeDescription(s, this->train, "train", 6);
	}

	void Dataset::writeShortDescription(std::ostream& s) const {
		s << "Dataset " << this->name << ": " << (this->isAngular() ? "angular" : "euclidean")
			<< " space, dimension = " << this->dim << ", trainCount = " << this->trainCount
			<< ", testCount = " << this->testCount << ", k = " << this->k << '\n';
	}

	void throwCouldNotOpen(const fs::path& p) {
		std::stringstream s;
		s << "Could not open file " << p << '.';
		throw std::runtime_error(s.str());
	}
}
