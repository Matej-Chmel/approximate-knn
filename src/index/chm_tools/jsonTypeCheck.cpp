#include "jsonTypeCheck.hpp"

namespace chm {
	namespace literals {
		std::stringstream operator"" _f(const char* const s, const size_t _) {
			return std::stringstream() << s;
		}
	}

	std::stringstream configStream(const fs::path& p) {
		return "Config path: "_f << p << '\n';
	}

	nl::json getRoot(const fs::path& p) {
		std::ifstream s(p);

		if(!s.is_open())
			throw std::runtime_error(("Could not open file "_f << p << '.').str());

		const auto res = nl::json::parse(std::string(
			std::istreambuf_iterator(s), std::istreambuf_iterator<char>())
		);

		if(!res.is_object())
			throw std::runtime_error((
				configStream(p) << "Root element must be an JSON object."
			).str());
		return res;
	}

	void throwMissingKey(const std::string& key, const fs::path& path) {
		throw std::runtime_error(
			(configStream(path) << "Missing key: " << key << '.').str()
		);
	}
}
