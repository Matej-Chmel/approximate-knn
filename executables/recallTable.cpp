#include <cstdlib>
#include <fstream>
#include <iostream>
#include <streambuf>
#include "chm_tools/RecallTable.hpp"
#include "libs/json.hpp"
namespace fs = chm::fs;
namespace nl = nlohmann;

#ifndef REPO_DIR
	constexpr auto REPO_DIR = "";
#endif

struct Config {
	std::string dataset;
	chm::uint efConstruction;
	std::vector<chm::uint> efSearch;
	chm::uint mMax;
	chm::uint seed;
	chm::SIMDType simdType;
	bool useHeuristic;
	bool usePrefetch;

	Config(const fs::path& p) {
		std::ifstream stream(p);

		if(!stream.is_open())
			chm::throwCouldNotOpen(p);

		auto obj = nl::json::parse(std::string(
			std::istreambuf_iterator(stream), std::istreambuf_iterator<char>())
		);

		this->dataset = obj["dataset"];
		this->efConstruction = obj["efConstruction"];
		this->mMax = obj["mMax"];
		this->seed = obj["seed"];
		this->useHeuristic = obj["useHeuristic"];
		this->usePrefetch = obj["usePrefetch"];

		if(!obj.contains("SIMD") || obj["SIMD"].is_null())
			this->simdType = chm::SIMDType::NONE;
		else {
			const std::string simdStr = obj["SIMD"];
			this->simdType = chm::getSIMDType(simdStr);
		}

		for(const auto& item : obj["efSearch"])
			this->efSearch.push_back(item.get<chm::uint>());
	}
};

template<bool useHeuristic, bool usePrefetch>
inline void runRecallTable(const Config& cfg, const fs::path& dataDir) {
	chm::RecallTable<useHeuristic, usePrefetch> table(dataDir / (cfg.dataset + ".bin"), cfg.efSearch);
	table.run(std::cout, cfg.efConstruction, cfg.mMax, cfg.seed, cfg.simdType);
	table.print(std::cout);
}

int main() {
	try {
		const auto repoDir = fs::path(REPO_DIR);
		Config cfg(repoDir / "config" / "recallTableConfig.json");
		const auto dataDir = repoDir / "data";

		if(cfg.useHeuristic) {
			if(cfg.usePrefetch)
				runRecallTable<true, true>(cfg, dataDir);
			else
				runRecallTable<true, false>(cfg, dataDir);
		} else {
			if(cfg.usePrefetch)
				runRecallTable<false, true>(cfg, dataDir);
			else
				runRecallTable<false, false>(cfg, dataDir);
		}

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
