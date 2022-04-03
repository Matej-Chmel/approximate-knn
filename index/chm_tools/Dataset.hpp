#pragma once
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include "chm/Index.hpp"
#include "chm/recall.hpp"

namespace chm {
	namespace fs = std::filesystem;

	template<bool useHeuristic = false, bool usePrefetch = false>
	class Dataset {
		size_t dim;
		uint k;
		std::string name;
		SpaceKind space;
		uint testCount;
		uint trainCount;
		std::vector<uint> neighbors;
		std::vector<float> test, train;

	public:
		void build(const IndexPtr<useHeuristic, usePrefetch>& index) const;
		Dataset(const fs::path& p);
		IndexPtr<useHeuristic, usePrefetch> getIndex(
			const uint efConstruction = 200, const uint mMax = 16,
			const uint seed = 100, const SIMDType simdType = SIMDType::NONE
		) const;
		float getRecall(const KnnResults& results) const;
		bool isAngular() const;
		KnnResults query(const IndexPtr<useHeuristic, usePrefetch>& index, const uint efSearch) const;
		void writeLongDescription(const fs::path& p) const;
		void writeLongDescription(std::ostream& s) const;
		void writeShortDescription(std::ostream& s) const;
	};

	template<bool useHeuristic, bool usePrefetch>
	using DatasetPtr = std::shared_ptr<const Dataset<useHeuristic, usePrefetch>>;

	void throwCouldNotOpen(const fs::path& p);

	template<typename T>
	inline void readBinary(std::ifstream& s, T& value) {
		s.read(reinterpret_cast<std::ifstream::char_type*>(&value), sizeof(T));
	}

	template<typename T>
	inline void readBinary(std::ifstream& s, std::vector<T>& v, const size_t len) {
		v.resize(len);
		s.read(reinterpret_cast<std::ifstream::char_type*>(v.data()), len * sizeof(T));
	}

	template<typename T>
	inline void writeDescription(std::ostream& s, const std::vector<T>& v, const std::string& name) {
		s << name << "[length " << v.size() << "]\n";

		for(const auto& item : v)
			s << item << '\n';
	}

	template<typename T>
	inline void writeDescription(
		std::ostream& s, const std::vector<T>& v, const std::string& name,
		const std::streamsize decimalPlaces
	) {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		s << name << "[length " << v.size() << "]\n"
			<< std::fixed << std::setprecision(decimalPlaces) << std::showpoint;

		for(const auto& item : v)
			s << item << '\n';

		s.copyfmt(streamState);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Dataset<useHeuristic, usePrefetch>::build(
		const IndexPtr<useHeuristic, usePrefetch>& index
	) const {
		index->push(this->train.data(), this->trainCount);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline Dataset<useHeuristic, usePrefetch>::Dataset(const fs::path& p) : name(p.stem().string()) {
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

	template<bool useHeuristic, bool usePrefetch>
	inline IndexPtr<useHeuristic, usePrefetch> Dataset<useHeuristic, usePrefetch>::getIndex(
		const uint efConstruction, const uint mMax, const uint seed, const SIMDType simdType
	) const {
		return std::make_shared<Index<useHeuristic, usePrefetch>>(
			this->dim, this->trainCount, efConstruction, mMax, seed, simdType, this->space
		);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline float Dataset<useHeuristic, usePrefetch>::getRecall(const KnnResults& results) const {
		return chm::getRecall(this->neighbors.data(), results.getLabels(), this->testCount, this->k);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline bool Dataset<useHeuristic, usePrefetch>::isAngular() const {
		switch(this->space) {
			case SpaceKind::ANGULAR:
				return true;
			case SpaceKind::EUCLIDEAN:
				return false;
			default:
				throw std::runtime_error("Invalid space.");
		}
	}

	template<bool useHeuristic, bool usePrefetch>
	inline KnnResults Dataset<useHeuristic, usePrefetch>::query(
		const IndexPtr<useHeuristic, usePrefetch>& index, const uint efSearch
	) const {
		index->setEfSearch(efSearch);
		return index->queryBatch(this->test.data(), this->testCount, this->k);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Dataset<useHeuristic, usePrefetch>::writeLongDescription(const fs::path& p) const {
		std::ofstream s(p);
		this->writeLongDescription(s);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Dataset<useHeuristic, usePrefetch>::writeLongDescription(std::ostream& s) const {
		s << "angular: " << (this->isAngular() ? "True" : "False") << '\n'
			<< "dim: " << dim << '\n'
			<< "k: " << k << '\n'
			<< "testCount: " << testCount << '\n'
			<< "trainCount: " << trainCount << '\n';
		chm::writeDescription(s, this->neighbors, "neighbors");
		chm::writeDescription(s, this->test, "test", 6);
		chm::writeDescription(s, this->train, "train", 6);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Dataset<useHeuristic, usePrefetch>::writeShortDescription(std::ostream& s) const {
		s << "Dataset " << this->name << ": " << (this->isAngular() ? "angular" : "euclidean")
			<< " space, dimension = " << this->dim << ", trainCount = " << this->trainCount
			<< ", testCount = " << this->testCount << ", k = " << this->k << '\n';
	}
}
