#include <fstream>
#include <streambuf>
#include "RecallTable.hpp"

namespace chm {
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

	std::vector<RecallTableConfig> RecallTableConfig::getVectorFromJSON(
		const fs::path& p, const fs::path& dataDir
	) {
		std::ifstream stream(p);

		if(!stream.is_open())
			throwCouldNotOpen(p);

		const auto arr = nl::json::parse(std::string(
			std::istreambuf_iterator(stream), std::istreambuf_iterator<char>())
		);

		if(!arr.is_array())
			throw std::runtime_error("Root element of \"recallTable.json\" must be a JSON array.");

		std::vector<RecallTableConfig> res;

		for(const auto& item : arr)
			res.emplace_back(item, dataDir);

		return res;
	}

	RecallTableConfig::RecallTableConfig(const nl::json& obj, const fs::path& dataDir) {
		if(!obj.is_object())
			throw std::runtime_error("Index configuration must be a JSON object.");

		this->datasetPath = dataDir / (getJSONValue<std::string>(obj, "dataset") + ".bin");
		this->efConstruction = getJSONValue<uint>(obj, "efConstruction");
		this->indexTemplate = getIndexTemplate(getJSONValue<std::string>(obj, "template"));
		this->mMax = getJSONValue<uint>(obj, "mMax");
		this->seed = getJSONValue<uint>(obj, "seed");
		this->simdType = !obj.contains("SIMD") || obj["SIMD"].is_null()
			? chm::SIMDType::NONE : chm::getSIMDType(getJSONValue<std::string>(obj, "SIMD"));

		if(!obj.contains("efSearch"))
			throwMissingKey("efSearch");

		const auto& efSearchObj = obj.at("efSearch");

		if(!efSearchObj.is_array())
			throwWrongType("efSearch", "JSON array");

		for(const auto& item : efSearchObj) {
			if(!item.is_number_unsigned())
				throwWrongType("efSearch", "JSON array of unsigned integers");

			this->efSearchValues.push_back(item.get<uint>());
		}
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

	RecallTablePtr getRecallTable(const RecallTableConfig& cfg) {
		switch(cfg.indexTemplate) {
			case chm::IndexTemplate::HEURISTIC:
				return std::make_shared<RecallTable<HeuristicTemplate>>(cfg);
			case chm::IndexTemplate::NAIVE:
				return std::make_shared<RecallTable<NaiveTemplate>>(cfg);
			case chm::IndexTemplate::NO_BIT_ARRAY:
				return std::make_shared<RecallTable<NoBitArrayTemplate>>(cfg);
			case chm::IndexTemplate::PREFETCHING:
				return std::make_shared<RecallTable<PrefetchingTemplate>>(cfg);
			default:
				throw std::runtime_error("Invalid index template.");
		}
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

	void throwMissingKey(const std::string& key) {
		std::stringstream s;
		s << "Missing key \"" << key <<  "\" in index configuration object.";
		throw std::runtime_error(s.str());
	}

	void throwWrongType(const std::string& key, const std::string& expectedType) {
		std::stringstream s;
		s << "Value of key \"" << key << "\" is not of type " << expectedType << ".";
		throw std::runtime_error(s.str());
	}
}
