#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
namespace fs = std::filesystem;

using uint = unsigned int;

template<typename T>
void read(std::ifstream& s, T& value) {
	s.read(reinterpret_cast<std::ifstream::char_type*>(&value), sizeof(T));
}

template<typename T>
void read(std::ifstream& s, std::vector<T>& v, const size_t len) {
	v.resize(len);
	s.read(reinterpret_cast<std::ifstream::char_type*>(v.data()), len * sizeof(T));
}

template<typename T>
void write(std::ostream& s, std::vector<T>& v, const std::string& name) {
	s << name << "[length " << v.size() << "]\n";

	for(const auto& item : v)
		s << item << '\n';
}

template<typename T>
void write(std::ostream& s, std::vector<T>& v, const std::string& name, const std::streamsize decimalPlaces) {
	std::ios streamState(nullptr);
	streamState.copyfmt(s);

	s << name << "[length " << v.size() << "]\n"
		<< std::fixed << std::setprecision(decimalPlaces) << std::showpoint;

	for(const auto& item : v)
		s << item << '\n';

	s.copyfmt(streamState);
}

void run(const fs::path& dataDir, std::ostream& s) {
	const auto path = dataDir / "test.bin";
	std::ifstream file(path, std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("Could not open dataset.");

	bool angular;
	uint dim, k, testCount, trainCount;

	read(file, angular);
	read(file, dim);
	read(file, k);
	read(file, testCount);
	read(file, trainCount);

	s << "angular: " << angular << '\n'
		<< "dim: " << dim << '\n'
		<< "k: " << k << '\n'
		<< "testCount: " << testCount << '\n'
		<< "trainCount: " << trainCount << '\n';

	std::vector<uint> neighbors;
	std::vector<float> test, train;

	read(file, neighbors, size_t(k * testCount));
	write(s, neighbors, "neighbors");
	read(file, test, size_t(dim * testCount));
	write(s, test, "test", 6);
	read(file, train, size_t(dim * trainCount));
	write(s, train, "train", 6);
}

int main() {
	try {
		const auto dataDir = fs::path(INDEX_PATH) / "apps" / "data";
		std::ofstream file(dataDir / "testCpp.txt");
		run(dataDir, file);
	} catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
	}
	return 0;
}
