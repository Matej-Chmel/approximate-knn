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

int main() {
	try {
		const auto repoDir = fs::path(REPO_DIR);
		Config cfg(repoDir / "config" / "recallTableConfig.json");
		chm::RecallTable table(repoDir / "data" / (cfg.dataset + ".bin"), cfg.efSearch);
		table.run(std::cout, cfg.efConstruction, cfg.mMax, cfg.seed, cfg.simdType);
		table.print(std::cout);

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
