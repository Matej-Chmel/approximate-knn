#pragma once
#include <iomanip>
#include <ios>
#include "Bruteforce.hpp"
#include "chm/Index.hpp"
#include "chm/recall.hpp"
#include "jsonTypeCheck.hpp"
#include "Timer.hpp"

namespace chm {
	/**
	 * Datový soubor.
	 * @tparam T Šablona sestavovaného indexu. @see IndexTemplate.
	 */
	template<class T = NaiveTemplate>
	class Dataset {
		/**
		 * Počet dimenzí prostoru.
		 */
		size_t dim;
		/**
		 * Počet hledaných sousedů pro jeden dotaz.
		 */
		uint k;
		/**
		 * Název datového souboru bez přípony.
		 */
		std::string name;
		/**
		 * Druh prostoru dle metriky.
		 */
		SpaceKind space;
		/**
		 * Počet dotazů.
		 */
		uint testCount;
		/**
		 * Počet prvků v datové kolekce pro stavbu indexu.
		 */
		uint trainCount;
		/**
		 * Identity skutečných nejbližších sousedů pro všechny dotazy.
		 * @see Bruteforce
		 */
		std::vector<uint> neighbors;
		/**
		 * Pole vektorů dotazovaných prvků.
		 */
		std::vector<float> test;
		/**
		 * Datová kolekce pro stavbu indexu.
		 */
		std::vector<float> train;

	public:
		/**
		 * Typ sdíleného ukazatele na datový soubor.
		 */
		using Ptr = std::shared_ptr<const Dataset<T>>;

		/**
		 * Statická metoda pro získání již existujícího nebo
		 * vytvoření nového datového souboru.
		 * @param[in] configPath Cesta k souboru s JSON konfigurací.
		 * @param[in] dataDir Cesta k adresáři s datovými soubory.
		 * @param[in] datasets JSON objekt s popisem datových souborů.
		 * @param[in] name @ref name
		 * @param[out] s Stream pro zapisování stavu.
		 * @return Ukazatel na datový soubor.
		 */
		static Ptr getDataset(
			const fs::path& configPath, const fs::path& dataDir,
			const nl::json& datasets, const std::string& name,
			std::ostream& s
		);

		/**
		 * Vloží prvky z datové kolekce @ref train do indexu.
		 * @param[in] index Ukazatel na index.
		 */
		void build(const IndexPtr<T>& index) const;
		/**
		 * Konstruktor z již existujícího souboru.
		 * @param[in] p Cesta k souboru.
		 */
		Dataset(const fs::path& p);
		/**
		 * Konstruktor pro vytvoření nového datového souboru.
		 * @param[in] obj JSON objekt s popisem datového souboru.
		 * @param[in] configPath Cesta k souboru s JSON konfigurací.
		 * @param[in] datasetPath Cesta k datovému souboru.
		 * @param[out] s Stream pro zapisování stavu.
		 */
		Dataset(
			const nl::json& obj, const fs::path& configPath,
			const fs::path& datasetPath, std::ostream& s
		);
		/**
		 * Vrátí prázdný index.
		 * @param[in] efConstruction @ref Configuration::efConstruction
		 * @param[in] mMax @ref Configuration::mMax
		 * @param[in] seed Nastavení generátoru náhodných úrovní nových prvků.
		 * @param[in] simdType SIMD rozšíření použité pro výpočet vzdálenosti.
		 * @return Ukazatel na prázdný index.
		 */
		IndexPtr<T> getIndex(
			const uint efConstruction = 200, const uint mMax = 16,
			const uint seed = 100, const SIMDType simdType = SIMDType::NONE
		) const;
		/**
		 * Vrátí přesnost výsledků indexu.
		 * @param[in] results Výsledky indexu.
		 * @return Přesnost výsledků.
		 */
		float getRecall(const KnnResults& results) const;
		/**
		 * Vrátí krátký popis datového souboru.
		 * @return Krátký popis datového souboru.
		 */
		std::string getString() const;
		/**
		 * Vrátí vlajku využití kosinusové vzdálenosti.
		 * @return Vlajka využití kosinusové vzdálenosti.
		 */
		bool isAngular() const;
		/**
		 * Zpracuje kolekci dotazů.
		 * @param[in] index Ukazatel na index, který kolekci zpracuje.
		 * @param[in] efSearch @ref Configuration::efSearch
		 */
		KnnResults query(const IndexPtr<T>& index, const uint efSearch) const;
		/**
		 * Převede datový soubor na textový formát.
		 * @param[in] p Cesta k výstupnímu souboru.
		 */
		void writeLongDescription(const fs::path& p) const;
		/**
		 * Převede datový soubor na textový formát.
		 * @param[out] s Výstupní stream.
		 */
		void writeLongDescription(std::ostream& s) const;
		/**
		 * Zapíše krátký popis datového souboru do streamu.
		 * @param[out] s Výstupní stream.
		 */
		void writeShortDescription(std::ostream& s) const;
	};

	/**
	 * Vygeneruje náhodnou datovou kolekci.
	 * @param[out] v Výsledná datová kolekce.
	 * @param[in] count Počet generovaných prvků.
	 * @param[in] dim Počet dimenzí prostoru.
	 * @param[in] seed Nastavení generátoru náhodných čísel.
	 */
	void generateRandomData(std::vector<float>& v, const size_t count, const size_t dim, const uint seed);
	/**
	 * Přečte data z binárního souboru do proměnné.
	 * @tparam T Primitivní datový typ.
	 * @param[in] s Vstupní stream.
	 * @param[out] value Výstupní proměnná.
	 */
	template<typename T> void readBinary(std::ifstream& s, T& value);
	/**
	 * Přečte pole dat z binárního souboru do vektoru.
	 * @tparam T Primitivní datový typ.
	 * @param[in] s Vstupní stream.
	 * @param[out] v Výstupní vektor.
	 * @param[in] len Počet složek vektoru.
	 */
	template<typename T> void readBinary(std::ifstream& s, std::vector<T>& v, const size_t len);
	/**
	 * Zapíše data z proměnné do binárního souboru.
	 * @tparam T Primitivní datový typ.
	 * @param[out] s Výstupní stream.
	 * @param[in] value Vstupní proměnná.
	 */
	template<typename T> void writeBinary(std::ofstream& s, const T& value);
	/**
	 * Zapíše data vektoru do binárního souboru.
	 * @tparam T Primitivní datový typ.
	 * @param[out] s Výstupní stream.
	 * @param[in] v Vstupní vektor.
	 */
	template<typename T> void writeBinary(std::ofstream& s, const std::vector<T>& v);
	/**
	 * Převede pole dat na textový formát.
	 * @tparam T Primitivní datový typ.
	 * @param[out] s Výstupní stream.
	 * @param[in] v Vstupní pole.
	 * @param[in] name Název pole.
	 */
	template<typename T> void writeDescription(
		std::ostream& s, const std::vector<T>& v, const std::string& name
	);
	/**
	 * Převede pole dat na textový formát a upraví počet desetinných míst.
	 * @tparam T Primitivní datový typ.
	 * @param[out] s Výstupní stream.
	 * @param[in] v Vstupní pole.
	 * @param[in] name Název pole.
	 * @param[in] decimalPlaces Počet desetinných míst.
	 */
	template<typename T> void writeDescription(
		std::ostream& s, const std::vector<T>& v, const std::string& name,
		const std::streamsize decimalPlaces
	);

	template<typename T>
	inline void readBinary(std::ifstream& s, T& value) {
		s.read(reinterpret_cast<std::ifstream::char_type*>(&value), sizeof(T));
	}

	template<typename T>
	inline void readBinary(std::ifstream& s, std::vector<T>& v, const size_t len) {
		v.resize(len);
		s.read(reinterpret_cast<std::ifstream::char_type*>(v.data()), len * sizeof(T));
	}

	template<typename T>
	inline void writeBinary(std::ofstream& s, const T& value) {
		s.write(reinterpret_cast<const std::ofstream::char_type*>(&value), sizeof(T));
	}

	template<typename T>
	inline void writeBinary(std::ofstream& s, const std::vector<T>& v) {
		s.write(reinterpret_cast<const std::ofstream::char_type*>(v.data()), v.size() * sizeof(T));
	}

	template<typename T>
	inline void writeDescription(
		std::ostream& s, const std::vector<T>& v, const std::string& name
	) {
		s << name << "[length " << v.size() << "]\n";

		for(const auto& item : v)
			s << item << '\n';
	}

	template<typename T>
	inline void writeDescription(
		std::ostream& s, const std::vector<T>& v, const std::string& name,
		const std::streamsize decimalPlaces
	) {
		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		s << name << "[length " << v.size() << "]\n"
			<< std::fixed << std::setprecision(decimalPlaces) << std::showpoint;

		for(const auto& item : v)
			s << item << '\n';

		s.copyfmt(streamState);
	}

	template<class T>
	inline typename Dataset<T>::Ptr Dataset<T>::getDataset(
		const fs::path& configPath, const fs::path& dataDir,
		const nl::json& datasets, const std::string& name,
		std::ostream& s
	) {
		const auto datasetPath = dataDir / (name + ".bin");

		if(fs::exists(datasetPath))
			return std::make_shared<const Dataset<T>>(datasetPath);

		for(const auto& obj : datasets) {
			if(getJSONValue<std::string>(obj, "name", configPath) == name)
				return std::make_shared<const Dataset<T>>(obj, configPath, datasetPath, s);
		}

		throw std::runtime_error(("Could not find dataset "_f << name).str());
		return nullptr;
	}

	template<class T>
	inline void Dataset<T>::build(
		const IndexPtr<T>& index
	) const {
		index->push(this->train.data(), this->trainCount);
	}

	template<class T>
	inline Dataset<T>::Dataset(const fs::path& p) : name(p.stem().string()) {
		std::ifstream file(p, std::ios::binary);

		if(!file.is_open())
			throwCouldNotOpen(p);

		bool angular;
		uint dim;
		readBinary(file, angular);
		this->space = angular ? SpaceKind::ANGULAR : SpaceKind::EUCLIDEAN;
		readBinary(file, dim);
		this->dim = size_t(dim);
		readBinary(file, this->k);
		readBinary(file, this->testCount);
		readBinary(file, this->trainCount);
		readBinary(file, this->neighbors, size_t(this->k) * size_t(this->testCount));
		readBinary(file, this->test, this->dim * size_t(this->testCount));
		readBinary(file, this->train, this->dim * size_t(this->trainCount));
	}

	template<class T>
	inline Dataset<T>::Dataset(
		const nl::json& obj, const fs::path& configPath,
		const fs::path& datasetPath, std::ostream& s
	) {
		const bool angular = getJSONValue<bool>(obj, "angular", configPath);
		this->space = angular ? SpaceKind::ANGULAR : SpaceKind::EUCLIDEAN;
		this->dim = getJSONValue<size_t>(obj, "dim", configPath);
		this->k = getJSONValue<uint>(obj, "k", configPath);
		this->name = getJSONValue<std::string>(obj, "name", configPath);
		const uint seed = getJSONValue<uint>(obj, "seed", configPath);
		this->testCount = getJSONValue<uint>(obj, "testCount", configPath);
		this->trainCount = getJSONValue<uint>(obj, "trainCount", configPath);

		{
			s << "Generating dataset.\n";
			Timer timer{};
			generateRandomData(this->test, this->testCount, this->dim, seed + 1);
			generateRandomData(this->train, this->trainCount, this->dim, seed);
			const auto elapsed = timer.getElapsed();
			s << "Dataset generated in ";
			prettyPrint(elapsed, s);
		}

		{
			s << "\nComputing nearest neighbors with bruteforce.\n";
			Timer timer{};
			Bruteforce bf(this->dim, this->k, this->trainCount, SIMDType::BEST, this->space);
			bf.push(this->train);
			bf.query(this->test, this->neighbors);
			const auto elapsed = timer.getElapsed();
			s << "Nearest neighbors computed in ";
			prettyPrint(elapsed, s);
		}

		const auto dataDir = datasetPath.parent_path();

		if(!fs::exists(dataDir))
			fs::create_directories(dataDir);

		s << "\nWriting dataset.\n";
		{
			std::ofstream datasetFile(datasetPath, std::ios::binary);
			writeBinary(datasetFile, angular);
			writeBinary(datasetFile, uint(this->dim));
			writeBinary(datasetFile, this->k);
			writeBinary(datasetFile, this->testCount);
			writeBinary(datasetFile, this->trainCount);
			writeBinary(datasetFile, this->neighbors);
			writeBinary(datasetFile, this->test);
			writeBinary(datasetFile, this->train);
		}
		s << "Dataset written to " << datasetPath << '\n';
	}

	template<class T>
	inline IndexPtr<T> Dataset<T>::getIndex(
		const uint efConstruction, const uint mMax, const uint seed, const SIMDType simdType
	) const {
		return std::make_shared<Index<T>>(
			this->dim, this->trainCount, efConstruction, mMax, seed, simdType, this->space
		);
	}

	template<class T>
	inline float Dataset<T>::getRecall(const KnnResults& results) const {
		return chm::getRecall(this->neighbors.data(), results.getLabels(), this->testCount, this->k);
	}

	template<class T>
	inline std::string Dataset<T>::getString() const {
		std::stringstream s;
		this->writeShortDescription(s);
		return s.str();
	}

	template<class T>
	inline bool Dataset<T>::isAngular() const {
		switch(this->space) {
			case SpaceKind::ANGULAR:
				return true;
			case SpaceKind::EUCLIDEAN:
				return false;
			default:
				throw std::runtime_error("Invalid space.");
		}
	}

	template<class T>
	inline KnnResults Dataset<T>::query(
		const IndexPtr<T>& index, const uint efSearch
	) const {
		index->setEfSearch(efSearch);
		return index->queryBatch(this->test.data(), this->testCount, this->k);
	}

	template<class T>
	inline void Dataset<T>::writeLongDescription(const fs::path& p) const {
		std::ofstream s(p);
		this->writeLongDescription(s);
	}

	template<class T>
	inline void Dataset<T>::writeLongDescription(std::ostream& s) const {
		s << "angular: " << (this->isAngular() ? "True" : "False") << '\n'
			<< "dim: " << dim << '\n'
			<< "k: " << k << '\n'
			<< "testCount: " << testCount << '\n'
			<< "trainCount: " << trainCount << '\n';
		chm::writeDescription(s, this->neighbors, "neighbors");
		chm::writeDescription(s, this->test, "test", 6);
		chm::writeDescription(s, this->train, "train", 6);
	}

	template<class T>
	inline void Dataset<T>::writeShortDescription(std::ostream& s) const {
		s << "Dataset " << this->name << ": " << (this->isAngular() ? "angular" : "euclidean")
			<< " space, dimension = " << this->dim << ", trainCount = " << this->trainCount
			<< ", testCount = " << this->testCount << ", k = " << this->k;
	}
}
