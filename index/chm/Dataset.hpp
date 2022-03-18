#pragma once
#include <filesystem>
#include <iomanip>
#include <ios>
#include <ostream>
#include "Index.hpp"

namespace chm {
	namespace fs = std::filesystem;

	class Dataset {
		size_t dim;
		uint k;
		SpaceKind space;
		uint testCount;
		uint trainCount;
		std::vector<uint> neighbors;
		std::vector<float> test, train;

	public:
		void build(const IndexPtr& index) const;
		Dataset(const fs::path& p);
		IndexPtr getIndex(const uint efConstruction, const uint mMax, const uint seed) const;
		float getRecall(const KnnResults& results) const;
		KnnResults query(const IndexPtr& index, const uint efSearch) const;
		void writeDescription(const fs::path& p) const;
		void writeDescription(std::ostream& s) const;
	};

	template<typename T>
	void readBinary(std::ifstream& s, T& value) {
		s.read(reinterpret_cast<std::ifstream::char_type*>(&value), sizeof(T));
	}

	template<typename T>
	void readBinary(std::ifstream& s, std::vector<T>& v, const size_t len) {
		v.resize(len);
		s.read(reinterpret_cast<std::ifstream::char_type*>(v.data()), len * sizeof(T));
	}

	template<typename T>
	void writeDescription(std::ostream& s, const std::vector<T>& v, const std::string& name) {
		s << name << "[length " << v.size() << "]\n";

		for(const auto& item : v)
			s << item << '\n';
	}

	template<typename T>
	void writeDescription(std::ostream& s, const std::vector<T>& v, const std::string& name, const std::streamsize decimalPlaces) {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		s << name << "[length " << v.size() << "]\n"
			<< std::fixed << std::setprecision(decimalPlaces) << std::showpoint;

		for(const auto& item : v)
			s << item << '\n';

		s.copyfmt(streamState);
	}
}
