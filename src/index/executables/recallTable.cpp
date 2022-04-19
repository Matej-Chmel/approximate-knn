#include <cstdlib>
#include <iostream>
#include "chm_tools/RecallTable.hpp"
namespace fs = std::filesystem;

#ifndef SRC_DIR
	constexpr auto SRC_DIR = "";
#endif

template<class T>
inline void runRecallTable(const chm::RecallTableConfig& cfg) {
	chm::RecallTable<T> table(cfg);
	table.run(std::cout);
	table.print(std::cout);
}

void runRecallTable(const fs::path& cfgPath, const fs::path& dataDir) {
	chm::RecallTableConfig cfg(cfgPath, dataDir);

	switch(cfg.indexTemplate) {
		case chm::IndexTemplate::HEURISTIC:
			runRecallTable<chm::HeuristicTemplate>(cfg);
			break;
		case chm::IndexTemplate::NAIVE:
			runRecallTable<chm::NaiveTemplate>(cfg);
			break;
		case chm::IndexTemplate::NO_BIT_ARRAY:
			runRecallTable<chm::NoBitArrayTemplate>(cfg);
			break;
		case chm::IndexTemplate::PREFETCHING:
			runRecallTable<chm::PrefetchingTemplate>(cfg);
			break;
		default:
			throw std::runtime_error("Invalid index template.");
	}
}

int main() {
	try {
		const auto srcDir = fs::path(SRC_DIR);
		const auto cfgPath = srcDir / "config" / "recallTableConfig.json";
		const auto dataDir = srcDir / "data";
		runRecallTable(cfgPath, dataDir);

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
