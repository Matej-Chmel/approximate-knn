#include <cstdlib>
#include <iostream>
#include "chm_tools/RecallTable.hpp"
namespace fs = std::filesystem;

#ifndef SRC_DIR
	constexpr auto SRC_DIR = "";
#endif

int main() {
	try {
		const auto srcDir = fs::path(SRC_DIR);
		const auto cfgPath = srcDir / "config" / "recallTable.json";
		const auto dataDir = srcDir / "data";
		const auto configs = chm::RecallTableConfig::getVectorFromJSON(cfgPath, dataDir);

		std::vector<chm::RecallTablePtr> tables;

		for(const auto& cfg : configs) {
			chm::RecallTablePtr t = chm::getRecallTable(cfg);
			t->run(std::cout);
			tables.push_back(t);
		}
		for(const auto& t : tables) {
			t->print(std::cout);
			std::cout << '\n';
		}

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
