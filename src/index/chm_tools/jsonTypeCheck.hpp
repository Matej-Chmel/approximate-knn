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
		/**
		 * Literál, který převede řetězec na stringstream.
		 * @param[in] s Řetězec.
		 * @return Stringstream obsahující řetězec @p s.
		 */
		std::stringstream operator"" _f(const char* const s, const size_t _);
	}

	using namespace literals;

	/**
	 * Vrátí chybový text s cestou konfigurace.
	 * @param[in] p Cesta ke konfiguračnímu souboru.
	 * @return Stream obsahující chybový text.
	 */
	std::stringstream configStream(const fs::path& p);
	/**
	 * Pokusí se o získání pole z JSON objektu.
	 * @tparam T Primitivní typ položky pole.
	 * @param[in] obj JSON objekt.
	 * @param[in] key Klíč pole.
	 * @param[in] path Cesta původního JSON souboru.
	 * @return Pole.
	 */
	template<typename T> std::vector<T> getJSONArray(
		const nl::json& obj, const std::string& key, const fs::path& path
	);
	/**
	 * Pokusí se o získání hodnoty z JSON objektu.
	 * @tparam T Primitivní typ hodnoty.
	 * @param[in] obj JSON objekt.
	 * @param[in] key Klíč hodnoty.
	 * @param[in] path Cesta původního JSON souboru.
	 * @return Získaná hodnota.
	 */
	template<typename T> T getJSONValue(
		const nl::json& obj, const std::string& key, const fs::path& path
	);
	/**
	 * Pokusí se o získání objektu v JSON souboru.
	 * @param[in] p Cesta k JSON souboru.
	 * @return Kořenový JSON objekt.
	 */
	nl::json getRoot(const fs::path& p);
	/**
	 * Vyhodí výjimku při chybě během otevírání souboru.
	 * @param[in] p Cesta k souboru.
	 */
	void throwCouldNotOpen(const fs::path& p);
	/**
	 * Vyhodí výjimku, když hledaný klíč v JSON objektu neexistuje.
	 * @param[in] key Klíč.
	 * @param[in] path Cesta původního JSON souboru.
	 */
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
