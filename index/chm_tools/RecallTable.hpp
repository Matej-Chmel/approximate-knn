#pragma once
#include <chrono>
#include "Dataset.hpp"

namespace chm {
	namespace chr = std::chrono;

	constexpr std::streamsize EF_SEARCH_WIDTH = 8;
	constexpr std::streamsize ELAPSED_PRETTY_WIDTH = 29;
	constexpr std::streamsize ELAPSED_WIDTH = 29;
	constexpr std::streamsize RECALL_WIDTH = 12;

	class QueryBenchmark {
		chr::nanoseconds elapsed;
		float recall;

	public:
		const uint efSearch;

		long long getElapsedNum() const;
		float getRecall() const;
		void prettyPrintElapsed(std::ostream& s) const;
		QueryBenchmark(const uint efSearch);
		void setElapsed(const chr::nanoseconds& elapsed);
		void setRecall(const float recall);
	};

	template<bool useHeuristic = false, bool usePrefetch = false>
	class RecallTable {
		std::vector<QueryBenchmark> benchmarks;
		chr::nanoseconds buildElapsed;
		const DatasetPtr<useHeuristic, usePrefetch> dataset;
		std::vector<uint> efSearchValues;
		IndexPtr<useHeuristic, usePrefetch> index;

	public:
		RecallTable(
			const DatasetPtr<useHeuristic, usePrefetch>& dataset,
			const std::vector<uint>& efSearchValues
		);
		RecallTable(const fs::path& datasetPath, const std::vector<uint>& efSearchValues);
		void print(std::ostream& s) const;
		void run(
			std::ostream& s,
			const uint efConstruction = 200, const uint mMax = 16,
			const uint seed = 100, const SIMDType simdType = SIMDType::NONE
		);
	};

	class Timer {
		chr::steady_clock::time_point start;

	public:
		chr::nanoseconds getElapsed() const;
		void reset();
		Timer();
	};

	void prettyPrint(const chr::nanoseconds& elapsed, std::ostream& s);
	void print(const float number, std::ostream& s, const std::streamsize places = 2);
	void print(const long long number, std::ostream& s, const std::streamsize places = 2);

	template<typename T>
	inline long long convert(chr::nanoseconds& t) {
		const auto res = chr::duration_cast<T>(t);
		t -= res;
		return res.count();
	}

	template<typename T>
	inline void printField(const T& field, std::ostream& s, const std::streamsize width) {
		s << std::right << std::setw(width) << field;
	}

	template<bool useHeuristic, bool usePrefetch>
	inline RecallTable<useHeuristic, usePrefetch>::RecallTable(
		const DatasetPtr<useHeuristic, usePrefetch>& dataset, const std::vector<uint>& efSearchValues
	) : buildElapsed(0), dataset(dataset), efSearchValues(efSearchValues), index(nullptr) {

		this->benchmarks.reserve(this->efSearchValues.size());
	}

	template<bool useHeuristic, bool usePrefetch>
	inline RecallTable<useHeuristic, usePrefetch>::RecallTable(
		const fs::path& datasetPath, const std::vector<uint>& efSearchValues
	) : RecallTable(std::make_shared<Dataset>(datasetPath), efSearchValues) {}

	template<bool useHeuristic, bool usePrefetch>
	inline void RecallTable<useHeuristic, usePrefetch>::print(std::ostream& s) const {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		this->dataset->writeShortDescription(s);
		s << this->index->getString() << '\n';
		s << "Build time: [";
		prettyPrint(this->buildElapsed, s);
		s << "], " << this->buildElapsed.count() << " ns\n\n";

		printField("EfSearch", s, EF_SEARCH_WIDTH);
		printField("Recall", s, RECALL_WIDTH);
		printField("Elapsed (pretty)", s, ELAPSED_PRETTY_WIDTH);
		printField("Elapsed (ns)", s, ELAPSED_WIDTH);
		printField("\n", s, 1);

		for(const auto& benchmark : this->benchmarks) {
			printField(benchmark.efSearch, s, EF_SEARCH_WIDTH);
			s << std::right << std::setw(RECALL_WIDTH);
			chm::print(benchmark.getRecall(), s, 3);
			s << std::right << std::setw(ELAPSED_PRETTY_WIDTH);
			benchmark.prettyPrintElapsed(s);
			printField(benchmark.getElapsedNum(), s, ELAPSED_WIDTH);
			printField("\n", s, 1);
		}

		s.copyfmt(streamState);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void RecallTable<useHeuristic, usePrefetch>::run(
		std::ostream& s, const uint efConstruction, const uint mMax,
		const uint seed, const SIMDType simdType
	) {
		Timer timer{};
		this->benchmarks.clear();

		s << "Building index.\n";

		timer.reset();
		this->index = this->dataset->getIndex(efConstruction, mMax, seed, simdType);
		this->dataset->build(this->index);
		this->buildElapsed = timer.getElapsed();

		s << "Index built in ";
		prettyPrint(this->buildElapsed, s);
		s << "\n\n";

		for(const auto& efSearch : this->efSearchValues) {
			auto& benchmark = this->benchmarks.emplace_back(efSearch);

			s << "Querying with efSearch = " << efSearch << ".\n";

			timer.reset();
			const auto knnResults = this->dataset->query(this->index, efSearch);
			benchmark.setElapsed(timer.getElapsed());

			s << "Completed in ";
			benchmark.prettyPrintElapsed(s);
			s << "\nComputing recall.\n";

			timer.reset();
			benchmark.setRecall(this->dataset->getRecall(knnResults));
			const auto recallElapsed = timer.getElapsed();
			s << "Recall " << benchmark.getRecall() << " computed in ";
			prettyPrint(recallElapsed, s);
			s << "\n\n";
		}
	}
}
