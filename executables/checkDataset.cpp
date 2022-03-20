#include <cstdlib>
#include <iostream>
#include "chm_tools/Dataset.hpp"
namespace fs = chm::fs;

#ifndef REPO_DIR
	constexpr auto REPO_DIR = "";
#endif

int main() {
	try {
		const auto dataDir = fs::path(REPO_DIR) / "data";
		const auto datasetPath = dataDir / "test.bin";
		const auto descPath = dataDir / "testCpp.txt";
		chm::Dataset(datasetPath).writeLongDescription(descPath);
		std::cout << "Description of dataset " << datasetPath << " written to " << descPath << ".\n";

	} catch(const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
