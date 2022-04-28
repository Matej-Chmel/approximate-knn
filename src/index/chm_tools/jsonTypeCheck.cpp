#include "jsonTypeCheck.hpp"

namespace chm {
	std::string configStream(const fs::path& p) {
		std::stringstream s;
		s << "Config path: " << p << '\n';
		return s.str();
	}

	nl::json getRoot(const fs::path& p) {
		std::ifstream s(p);

		if(!s.is_open())
			throwCouldNotOpen(p);

		const auto res = nl::json::parse(std::string(
			std::istreambuf_iterator(s), std::istreambuf_iterator<char>())
		);

		if(!res.is_object()) {
			std::stringstream strStream;
			strStream << configStream(p) << "Root element must be an JSON object.";
			throw std::runtime_error(strStream.str());
		}
		return res;
	}

	void throwCouldNotOpen(const fs::path& p) {
		std::stringstream s;
		s << "Could not open file " << p << '.';
		throw std::runtime_error(s.str());
	}

	void throwMissingKey(const std::string& key, const fs::path& path) {
		std::stringstream s;
		s << configStream(path) << "Missing key: " << key << '.';
		throw std::runtime_error(s.str());
	}

	void throwWrongType(
		const std::string& key, const nl::json& val,
		const std::string& expectedType, const fs::path& path
	) {
		std::stringstream s;
		s << configStream(path) << "Key: " << key <<
			"\nExpected type: " << expectedType <<
			"\nActual type: " << val.type_name();
		throw std::runtime_error(s.str());
	}
}
