#pragma once
#include <chrono>
#include "Dataset.hpp"

namespace chm {
	namespace chr = std::chrono;

	class QueryBenchmark {
		chr::microseconds elapsed;
		float recall;

	public:
		const uint efSearch;

		long long getElapsedNum() const;
		float getRecall() const;
		void prettyPrintElapsed(std::ostream& s) const;
		QueryBenchmark(const uint efSearch);
		void setElapsed(const chr::microseconds& elapsed);
		void setRecall(const float recall);
	};

	class RecallTable {
		std::vector<QueryBenchmark> benchmarks;
		chr::microseconds buildElapsed;
		const DatasetPtr dataset;
		std::vector<uint> efSearchValues;
		IndexPtr index;

	public:
		RecallTable(const DatasetPtr& dataset, const std::vector<uint>& efSearchValues);
		RecallTable(const fs::path& datasetPath, const std::vector<uint>& efSearchValues);
		void print(std::ostream& s) const;
		void run(const uint efConstruction, const uint mMax, const uint seed, std::ostream& s);
	};

	class Timer {
		chr::steady_clock::time_point start;

	public:
		chr::microseconds getElapsed() const;
		void reset();
		Timer();
	};

	void prettyPrint(const chr::microseconds& elapsed, std::ostream& s);
	void print(const float number, std::ostream& s, const std::streamsize places = 2);
	void print(const long long number, std::ostream& s, const std::streamsize places = 2);

	template<typename T>
	long long convert(chr::microseconds& t) {
		const auto res = chr::duration_cast<T>(t);
		t -= res;
		return res.count();
	}

	template<typename T>
	void printField(const T& field, std::ostream& s, const std::streamsize width) {
		s << std::right << std::setw(width) << field;
	}
}
