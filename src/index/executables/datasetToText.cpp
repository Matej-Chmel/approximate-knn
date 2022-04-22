#include <cstdlib>
#include <iostream>
#include "chm_tools/Dataset.hpp"
namespace fs = chm::fs;

#ifndef SRC_DIR
	constexpr auto SRC_DIR = "";
#endif

int main(const int argsLen, const char* const* const args) {
	try {
		const auto dataDir = fs::absolute(fs::path(SRC_DIR)) / "data";
		const std::string datasetName = argsLen > 1 ? args[1] : "test";
		const auto datasetPath = dataDir / (datasetName + ".bin");
		const auto descPath = dataDir / (datasetName + ".cpp.txt");
		chm::Dataset(datasetPath).writeLongDescription(descPath);
		std::cout << "Description of dataset " << datasetPath << " written to " << descPath << ".\n";

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
