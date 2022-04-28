#pragma once
#include <iomanip>
#include <ios>
#include "Bruteforce.hpp"
#include "chm/Index.hpp"
#include "chm/recall.hpp"
#include "jsonTypeCheck.hpp"
#include "Timer.hpp"

namespace chm {
	template<class T = NaiveTemplate>
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
		using Ptr = std::shared_ptr<const Dataset<T>>;

		static Ptr getDataset(
			const fs::path& configPath, const fs::path& dataDir,
			const nl::json& datasets, const std::string& name,
			std::ostream& s
		);

		void build(const IndexPtr<T>& index) const;
		Dataset(const fs::path& p);
		Dataset(
			const nl::json& obj, const fs::path& configPath,
			const fs::path& datasetPath, std::ostream& s
		);
		IndexPtr<T> getIndex(
			const uint efConstruction = 200, const uint mMax = 16,
			const uint seed = 100, const SIMDType simdType = SIMDType::NONE
		) const;
		float getRecall(const KnnResults& results) const;
		std::string getString() const;
		bool isAngular() const;
		KnnResults query(const IndexPtr<T>& index, const uint efSearch) const;
		void writeLongDescription(const fs::path& p) const;
		void writeLongDescription(std::ostream& s) const;
		void writeShortDescription(std::ostream& s) const;
	};

	void generateRandomData(std::vector<float>& v, const size_t count, const size_t dim, const uint seed);
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
	inline void writeBinary(std::ofstream& s, const T& value) {
		s.write(reinterpret_cast<const std::ofstream::char_type*>(&value), sizeof(T));
	}

	template<typename T>
	inline void writeBinary(std::ofstream& s, std::vector<T>& v) {
		s.write(reinterpret_cast<const std::ofstream::char_type*>(v.data()), v.size() * sizeof(T));
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

	template<class T>
	inline typename Dataset<T>::Ptr Dataset<T>::getDataset(
		const fs::path& configPath, const fs::path& dataDir,
		const nl::json& datasets, const std::string& name,
		std::ostream& s
	) {
		const auto datasetPath = dataDir / (name + ".bin");

		if(fs::exists(datasetPath))
			return std::make_shared<const Dataset<T>>(datasetPath);

		for(const auto& obj : datasets) {
			if(getJSONValue<std::string>(obj, "name", configPath) == name)
				return std::make_shared<const Dataset<T>>(obj, configPath, datasetPath, s);
		}

		throw std::runtime_error(("Could not find dataset "_f << name).str());
		return nullptr;
	}

	template<class T>
	inline void Dataset<T>::build(
		const IndexPtr<T>& index
	) const {
		index->push(this->train.data(), this->trainCount);
	}

	template<class T>
	inline Dataset<T>::Dataset(const fs::path& p) : name(p.stem().string()) {
		std::ifstream file(p, std::ios::binary);

		if(!file.is_open())
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
		readBinary(file, this->neighbors, size_t(this->k) * size_t(this->testCount));
		readBinary(file, this->test, this->dim * size_t(this->testCount));
		readBinary(file, this->train, this->dim * size_t(this->trainCount));
	}

	template<class T>
	inline Dataset<T>::Dataset(
		const nl::json& obj, const fs::path& configPath,
		const fs::path& datasetPath, std::ostream& s
	) {
		const bool angular = getJSONValue<bool>(obj, "angular", configPath);
		this->space = angular ? SpaceKind::ANGULAR : SpaceKind::EUCLIDEAN;
		this->dim = getJSONValue<size_t>(obj, "dim", configPath);
		this->k = getJSONValue<uint>(obj, "k", configPath);
		this->name = getJSONValue<std::string>(obj, "name", configPath);
		const uint seed = getJSONValue<uint>(obj, "seed", configPath);
		this->testCount = getJSONValue<uint>(obj, "testCount", configPath);
		this->trainCount = getJSONValue<uint>(obj, "trainCount", configPath);

		{
			s << "Generating dataset.\n";
			Timer timer{};
			generateRandomData(this->test, this->testCount, this->dim, seed);
			generateRandomData(this->train, this->trainCount, this->dim, seed);
			const auto elapsed = timer.getElapsed();
			s << "Dataset generated in ";
			prettyPrint(elapsed, s);
		}

		{
			s << "\nComputing nearest neighbors with bruteforce.\n";
			Timer timer{};
			Bruteforce bf(this->dim, this->k, this->trainCount, SIMDType::BEST, this->space);
			bf.push(this->train);
			bf.query(this->test, this->neighbors);
			const auto elapsed = timer.getElapsed();
			s << "Nearest neighbors computed in ";
			prettyPrint(elapsed, s);
		}

		s << "\nWriting dataset.\n";
		{
			std::ofstream datasetFile(datasetPath);
			writeBinary(datasetFile, angular);
			writeBinary(datasetFile, this->dim);
			writeBinary(datasetFile, this->k);
			writeBinary(datasetFile, this->testCount);
			writeBinary(datasetFile, this->trainCount);
			writeBinary(datasetFile, this->neighbors);
			writeBinary(datasetFile, this->test);
			writeBinary(datasetFile, this->train);
		}
		s << "Dataset written to " << datasetPath << '\n';
	}

	template<class T>
	inline IndexPtr<T> Dataset<T>::getIndex(
		const uint efConstruction, const uint mMax, const uint seed, const SIMDType simdType
	) const {
		return std::make_shared<Index<T>>(
			this->dim, this->trainCount, efConstruction, mMax, seed, simdType, this->space
		);
	}

	template<class T>
	inline float Dataset<T>::getRecall(const KnnResults& results) const {
		return chm::getRecall(this->neighbors.data(), results.getLabels(), this->testCount, this->k);
	}

	template<class T>
	inline std::string Dataset<T>::getString() const {
		std::stringstream s;
		this->writeShortDescription(s);
		return s.str();
	}

	template<class T>
	inline bool Dataset<T>::isAngular() const {
		switch(this->space) {
			case SpaceKind::ANGULAR:
				return true;
			case SpaceKind::EUCLIDEAN:
				return false;
			default:
				throw std::runtime_error("Invalid space.");
		}
	}

	template<class T>
	inline KnnResults Dataset<T>::query(
		const IndexPtr<T>& index, const uint efSearch
	) const {
		index->setEfSearch(efSearch);
		return index->queryBatch(this->test.data(), this->testCount, this->k);
	}

	template<class T>
	inline void Dataset<T>::writeLongDescription(const fs::path& p) const {
		std::ofstream s(p);
		this->writeLongDescription(s);
	}

	template<class T>
	inline void Dataset<T>::writeLongDescription(std::ostream& s) const {
		s << "angular: " << (this->isAngular() ? "True" : "False") << '\n'
			<< "dim: " << dim << '\n'
			<< "k: " << k << '\n'
			<< "testCount: " << testCount << '\n'
			<< "trainCount: " << trainCount << '\n';
		chm::writeDescription(s, this->neighbors, "neighbors");
		chm::writeDescription(s, this->test, "test", 6);
		chm::writeDescription(s, this->train, "train", 6);
	}

	template<class T>
	inline void Dataset<T>::writeShortDescription(std::ostream& s) const {
		s << "Dataset " << this->name << ": " << (this->isAngular() ? "angular" : "euclidean")
			<< " space, dimension = " << this->dim << ", trainCount = " << this->trainCount
			<< ", testCount = " << this->testCount << ", k = " << this->k;
	}
}
