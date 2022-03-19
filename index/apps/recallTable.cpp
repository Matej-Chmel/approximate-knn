#include <cstdlib>
#include <iostream>
#include "chm/RecallTable.hpp"
namespace fs = chm::fs;

#ifndef INDEX_PATH
	constexpr auto INDEX_PATH = "";
#endif

constexpr auto DEFAULT_DATASET_NAME = "angular-10000";

int main(const int argsLen, const char* const * const args) {
	const std::string datasetName = argsLen > 1 ? args[1] : DEFAULT_DATASET_NAME;

	try {
		const auto dataDir = fs::path(INDEX_PATH) / "apps" / "data";
		chm::RecallTable table(dataDir / (datasetName + ".bin"), {10, 50, 100, 500});
		table.run(200, 16, 200, std::cout);
		table.print(std::cout);

	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
