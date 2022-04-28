#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <type_traits>
#include <vector>
#include "libs/json.hpp"

namespace chm {
	namespace fs = std::filesystem;
	namespace nl = nlohmann;

	namespace literals {
		std::stringstream operator"" _f(const char* const s, const size_t _);
	}

	using namespace literals;

	std::stringstream configStream(const fs::path& p);

	template<typename T>
	std::vector<T> getJSONArray(const nl::json& obj, const std::string& key, const fs::path& path);

	template<typename T>
	T getJSONValue(const nl::json& obj, const std::string& key, const fs::path& path);

	nl::json getRoot(const fs::path& p);
	void throwMissingKey(const std::string& key, const fs::path& path);

	template<typename T>
	inline T getJSONValue(const nl::json& obj, const std::string& key, const fs::path& path) {
		if(!obj.contains(key))
			throwMissingKey(key, path);

		try {
			return obj.at(key).get<T>();
		} catch(const nl::json::exception&) {
			throw std::runtime_error((
				configStream(path) << "Key: " << key <<
				"\nExpected type: " << typeid(T).name() <<
				"\nActual type: " << obj.at(key).type_name()
			).str());
		}
		return obj.at(key).get<T>();
	}

	template<typename T>
	inline std::vector<T> getJSONArray(
		const nl::json& obj, const std::string& key, const fs::path& path
	) {
		if(!obj.contains(key))
			throwMissingKey(key, path);

		const auto& arr = obj.at(key);

		if(!arr.is_array())
			throw std::runtime_error((
				configStream(path) << "Key: " << key <<
				"\nExpected type: JSON array\nActual type: " << arr.type_name()
			).str());

		std::vector<T> res;

		if constexpr(std::is_same<T, nl::json>::value) {
			for(const auto& item : arr) {
				if(!item.is_object())
					throw std::runtime_error((
						configStream(path) << "Key: " << key <<
						"\nExpected type: JSON object\nActual type: " << item.type_name()
					).str());
				res.push_back(item);
			}
		} else {
			try {
				for(const auto& item : arr)
					res.push_back(item.get<T>());
			} catch(const nl::json::exception&) {
				throw std::runtime_error((
					configStream(path) << "Key: " << key <<
					"\nExpected type: JSON array of " << typeid(T).name()
				).str());
			}
		}
		return res;
	}
}
