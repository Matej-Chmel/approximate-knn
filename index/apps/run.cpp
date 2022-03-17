#include <iostream>
#include "chm/Configuration.hpp"

int main() {
	chm::Configuration cfg(200, 16);
	std::cout << cfg.efConstruction << '\n';
	return 0;
}
