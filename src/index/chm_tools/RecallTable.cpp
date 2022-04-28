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
		const nl::json& arr, const fs::path& configPath
	) {
		std::vector<RecallTableConfig> res;

		for(const auto& item : arr)
			res.emplace_back(item, configPath);

		return res;
	}

	RecallTableConfig::RecallTableConfig(const nl::json& obj, const fs::path& configPath) {
		this->datasetName = getJSONValue<std::string>(obj, "dataset", configPath);
		this->efConstruction = getJSONValue<uint>(obj, "efConstruction", configPath);
		this->efSearchValues = getJSONArray<uint>(obj, "efSearch", configPath);
		this->indexTemplate = getIndexTemplate(getJSONValue<std::string>(obj, "template", configPath));
		this->mMax = getJSONValue<uint>(obj, "mMax", configPath);
		this->seed = getJSONValue<uint>(obj, "seed", configPath);

		this->simdType = !obj.contains("SIMD") || obj["SIMD"].is_null()
			? chm::SIMDType::NONE : chm::getSIMDType(getJSONValue<std::string>(obj, "SIMD", configPath));
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
}
