#include "RecallTable.hpp"

namespace chm {
	constexpr std::streamsize EF_SEARCH_WIDTH = 8;
	constexpr std::streamsize ELAPSED_PRETTY_WIDTH = 16;
	constexpr std::streamsize ELAPSED_WIDTH = 20;
	constexpr std::streamsize RECALL_WIDTH = 20;

	long long QueryBenchmark::getElapsedNum() const {
		return this->elapsed.count();
	}

	float QueryBenchmark::getRecall() const {
		return this->recall;
	}

	void QueryBenchmark::prettyPrintElapsed(std::ostream& s) const {
		prettyPrint(this->elapsed, s);
	}

	QueryBenchmark::QueryBenchmark(const uint efSearch) : efSearch(efSearch), elapsed(0), recall(0.f) {}

	void QueryBenchmark::setElapsed(const chr::microseconds& elapsed) {
		this->elapsed = elapsed;
	}

	void QueryBenchmark::setRecall(const float recall) {
		this->recall = recall;
	}

	RecallTable::RecallTable(const DatasetPtr& dataset, const std::vector<uint>& efSearchValues)
		: buildElapsed(0), dataset(dataset), efSearchValues(efSearchValues), index(nullptr) {

		this->benchmarks.reserve(this->efSearchValues.size());
	}

	RecallTable::RecallTable(const fs::path& datasetPath, const std::vector<uint>& efSearchValues)
		: RecallTable(std::make_shared<Dataset>(datasetPath), efSearchValues) {}

	void RecallTable::print(std::ostream& s) const {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		this->dataset->writeShortDescription(s);
		s << this->index->getString() << '\n';
		s << "Build time: [";
		prettyPrint(this->buildElapsed, s);
		s << "], " << this->buildElapsed.count() << " us\n\n";

		s << std::setw(EF_SEARCH_WIDTH) << "EfSearch"
			<< std::setw(RECALL_WIDTH) << "Recall"
			<< std::setw(ELAPSED_PRETTY_WIDTH) << "Elapsed (pretty)"
			<< std::setw(ELAPSED_WIDTH) << "Elapsed (us)"
			<< std::setw(1) << '\n';

		for(const auto& benchmark : this->benchmarks) {
			s << std::setw(EF_SEARCH_WIDTH) << benchmark.efSearch << std::setw(RECALL_WIDTH);
			chm::print(benchmark.getRecall(), s, 3);
			s << std::setw(ELAPSED_PRETTY_WIDTH);
			benchmark.prettyPrintElapsed(s);
			s << std::setw(ELAPSED_WIDTH) << benchmark.getElapsedNum() << std::setw(1) << '\n';
		}

		s.copyfmt(streamState);
	}

	void RecallTable::run(const uint efConstruction, const uint mMax, const uint seed, std::ostream& s) {
		Timer timer{};
		this->benchmarks.clear();

		s << "Building index.\n";

		timer.reset();
		this->index = this->dataset->getIndex(efConstruction, mMax, seed);
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

	chr::microseconds Timer::getElapsed() const {
		return chr::duration_cast<chr::microseconds>(chr::steady_clock::now() - this->start);
	}

	void Timer::reset() {
		this->start = chr::steady_clock::now();
	}

	Timer::Timer() {
		this->reset();
	}

	void prettyPrint(const chr::microseconds& elapsed, std::ostream& s) {
		chr::microseconds elapsedCopy = elapsed;
		const auto hours = convert<chr::hours>(elapsedCopy);
		const auto mins = convert<chr::minutes>(elapsedCopy);
		const auto secs = convert<chr::seconds>(elapsedCopy);
		const auto ms = convert<chr::milliseconds>(elapsedCopy);

		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		print(hours, s);
		s << ':';
		print(mins, s);
		s << ':';
		print(secs, s);
		s << '.';
		print(ms, s);
		s << '.';
		print(elapsedCopy.count(), s);

		s.copyfmt(streamState);
	}

	void print(const float number, std::ostream& s, const std::streamsize places = 2) {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);
		s << std::fixed << std::showpoint << std::setprecision(places) << number;
		s.copyfmt(streamState);
	}

	void print(const long long number, std::ostream& s, const std::streamsize places = 2) {
		s << std::setfill('0') << std::setw(places) << number;
	}
}
