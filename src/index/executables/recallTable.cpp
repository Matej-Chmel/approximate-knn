#include <cstdlib>
#include <iostream>
#include "chm_tools/RecallTable.hpp"

#ifndef SRC_DIR
	constexpr auto SRC_DIR = "";
#endif

int main(const int argsLen, const char* const * const args) {
	using namespace chm;

	try {
		const auto srcDir = fs::weakly_canonical(fs::path(SRC_DIR));
		const auto cfgPath = argsLen > 1
			? fs::weakly_canonical(fs::path(args[1]))
			: srcDir / "config" / "config.json";
		const auto dataDir = srcDir / "data";

		const auto configRoot = getRoot(cfgPath);
		const auto datasets = getJSONArray<nl::json>(configRoot, "datasets", cfgPath);
		const auto indexConfigs = getJSONArray<nl::json>(configRoot, "index", cfgPath);
		const auto configs = chm::RecallTableConfig::getVectorFromJSON(indexConfigs, cfgPath);

		std::vector<chm::RecallTablePtr> tables;

		for(const auto& cfg : configs) {
			chm::RecallTablePtr t = chm::getRecallTable(cfg);
			t->run(cfgPath, dataDir, datasets, std::cout);
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
