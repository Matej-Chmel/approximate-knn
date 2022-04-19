#include <fstream>
#include <streambuf>
#include "RecallTable.hpp"

namespace chm {
	namespace nl = nlohmann;

	long long QueryBenchmark::getElapsedNum() const {
		return this->elapsed.count();
	}

	float QueryBenchmark::getRecall() const {
		return this->recall;
	}

	void QueryBenchmark::prettyPrintElapsed(std::ostream& s) const {
		std::stringstream strStream;
		prettyPrint(this->elapsed, strStream);
		s << strStream.str();
	}

	QueryBenchmark::QueryBenchmark(const uint efSearch) : efSearch(efSearch), elapsed(0), recall(0.f) {}

	void QueryBenchmark::setElapsed(const chr::nanoseconds& elapsed) {
		this->elapsed = elapsed;
	}

	void QueryBenchmark::setRecall(const float recall) {
		this->recall = recall;
	}

	RecallTableConfig::RecallTableConfig(const fs::path& jsonPath, const fs::path& dataDir) {
		std::ifstream stream(jsonPath);

		if(!stream.is_open())
			throwCouldNotOpen(jsonPath);

		const auto obj = nl::json::parse(std::string(
			std::istreambuf_iterator(stream), std::istreambuf_iterator<char>())
		);

		this->datasetPath = dataDir / (obj.at("dataset").get<std::string>() + ".bin");
		this->efConstruction = obj["efConstruction"];
		this->indexTemplate = getIndexTemplate(obj.at("template").get<std::string>());
		this->mMax = obj["mMax"];
		this->seed = obj["seed"];

		if(!obj.contains("SIMD") || obj["SIMD"].is_null())
			this->simdType = chm::SIMDType::NONE;
		else {
			const std::string simdStr = obj["SIMD"];
			this->simdType = chm::getSIMDType(simdStr);
		}

		for(const auto& item : obj["efSearch"])
			this->efSearchValues.push_back(item.get<chm::uint>());
	}

	chr::nanoseconds Timer::getElapsed() const {
		return chr::duration_cast<chr::nanoseconds>(chr::steady_clock::now() - this->start);
	}

	void Timer::reset() {
		this->start = chr::steady_clock::now();
	}

	Timer::Timer() {
		this->reset();
	}

	void prettyPrint(const chr::nanoseconds& elapsed, std::ostream& s) {
		chr::nanoseconds elapsedCopy = elapsed;
		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		print(convert<chr::hours>(elapsedCopy), s);
		s << ':';
		print(convert<chr::minutes>(elapsedCopy), s);
		s << ':';
		print(convert<chr::seconds>(elapsedCopy), s);
		s << '.';
		print(convert<chr::milliseconds>(elapsedCopy), s, 3);
		s << '.';
		print(convert<chr::microseconds>(elapsedCopy), s, 3);
		s << '.';
		print(elapsedCopy.count(), s, 3);

		s.copyfmt(streamState);
	}

	void print(const float number, std::ostream& s, const std::streamsize places) {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);
		s << std::fixed << std::showpoint << std::setprecision(places) << number;
		s.copyfmt(streamState);
	}

	void print(const long long number, std::ostream& s, const std::streamsize places) {
		s << std::setfill('0') << std::setw(places) << number;
	}
}