#include <cstdlib>
#include <fstream>
#include <iostream>
#include <streambuf>
#include "chm_tools/RecallTable.hpp"
#include "libs/json.hpp"
namespace fs = chm::fs;

#ifndef INDEX_PATH
	constexpr auto INDEX_PATH = "";
#endif

struct Config {
	std::string dataset;
	chm::uint efConstruction;
	std::vector<chm::uint> efSearch;
	chm::uint mMax;
	chm::uint seed;

	Config(const fs::path& p) {
		std::ifstream stream(p);

		if(!stream.is_open())
			chm::throwCouldNotOpen(p);

		auto obj = nlohmann::json::parse(std::string(
			std::istreambuf_iterator(stream), std::istreambuf_iterator<char>())
		);
		this->dataset = obj["dataset"];
		this->efConstruction = obj["efConstruction"];
		this->mMax = obj["mMax"];
		this->seed = obj["seed"];

		for(const auto& item : obj["efSearch"])
			this->efSearch.push_back(item.get<chm::uint>());
	}
};

int main() {
	try {
		const auto appsDir = fs::path(INDEX_PATH) / "apps";
		Config cfg(appsDir / "config" / "recallTableConfig.json");
		chm::RecallTable table(appsDir / "data" / (cfg.dataset + ".bin"), cfg.efSearch);
		table.run(cfg.efConstruction, cfg.mMax, cfg.seed, std::cout);
		table.print(std::cout);

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
