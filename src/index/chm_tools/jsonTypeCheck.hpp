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

	/**
	 * Vrátí chybový text s cestou konfigurace.
	 * @param[in] p Cesta ke konfiguračnímu souboru.
	 * @return Chybový text.
	 */
	std::string configStream(const fs::path& p);
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
	/**
	 * Vyhodí výjimku, když hodnota v JSON objektu není správného typu.
	 * @param[in] key Klíč hodnoty.
	 * @param[in] val Hodnota.
	 * @param[in] expectedType Očekávaný typ.
	 * @param[in] path Cesta původního JSON souboru.
	 */
	void throwWrongType(
		const std::string& key, const nl::json& val,
		const std::string& expectedType, const fs::path& path
	);

	template<typename T>
	inline T getJSONValue(const nl::json& obj, const std::string& key, const fs::path& path) {
		if(!obj.contains(key))
			throwMissingKey(key, path);

		const auto& val = obj.at(key);

		try {
			return val.get<T>();
		} catch(const nl::json::exception&) {
			throwWrongType(key, val, typeid(T).name(), path);
		}
		return T();
	}

	template<typename T>
	inline std::vector<T> getJSONArray(
		const nl::json& obj, const std::string& key, const fs::path& path
	) {
		if(!obj.contains(key))
			throwMissingKey(key, path);

		const auto& arr = obj.at(key);

		if(!arr.is_array())
			throwWrongType(key, arr, "JSON type", path);

		std::vector<T> res;

		if constexpr(std::is_same<T, nl::json>::value)
			for(const auto& item : arr) {
				if(!item.is_object())
					throwWrongType(key, item, "JSON object", path);
				res.push_back(item);
			}
		else
			try {
				for(const auto& item : arr)
					res.push_back(item.get<T>());
			} catch(const nl::json::exception&) {
				std::stringstream s;
				s << configStream(path) << "Key: " << key <<
					"\nExpected type: JSON array of " << typeid(T).name();
				throw std::runtime_error(s.str());
			}
		return res;
	}
}
