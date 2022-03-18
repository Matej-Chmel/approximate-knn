#include <cstdlib>
#include <iostream>
#include "chm/RecallTable.hpp"
namespace fs = chm::fs;

#ifndef INDEX_PATH
	constexpr auto INDEX_PATH = "";
#endif

int main() {
	try {
		const auto dataDir = fs::path(INDEX_PATH) / "apps" / "data";
		chm::RecallTable table(dataDir / "test.bin", {3, 4, 6});
		table.run(10, 5, 100, std::cout);
		table.print(std::cout);

	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
