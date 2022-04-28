#pragma once
#include "Dataset.hpp"

namespace chm {
	/**
	 * Maximální počet znaků vyhrazený v tabulce
	 * pro zápis hodnoty parametru vyhledávání @ref Configuration::efSearch.
	 */
	constexpr std::streamsize EF_SEARCH_WIDTH = 8;
	/**
	 * Maximální počet znaků vyhrazený v tabulce
	 * pro pěkný výpis uplynulého času.
	 */
	constexpr std::streamsize ELAPSED_PRETTY_WIDTH = 29;
	/**
	 * Maximální počet znaků vyhrazený v tabulce
	 * pro obyčejný výpis uplynulého času.
	 */
	constexpr std::streamsize ELAPSED_WIDTH = 29;
	/**
	 * Maximální počet znaků vyhrazený v tabulce
	 * pro zápis hodnoty přesnosti.
	 */
	constexpr std::streamsize RECALL_WIDTH = 12;

	/**
	 * Objekt popisující kvalitu zpracování kolekce dotazů.
	 */
	class QueryBenchmark {
		/**
		 * Uplynulý čas v nanosekundách.
		 */
		chr::nanoseconds elapsed;
		/**
		 * Přesnost vyhledávání.
		 */
		float recall;

	public:
		/**
		 * Hodnota parametru vyhledávání @ref Configuration::efSearch.
		 */
		const uint efSearch;

		/**
		 * Vrátí počet uplynulých nanosekund.
		 * @return Počet uplynulých nanosekund.
		 */
		long long getElapsedNum() const;
		/**
		 * Vrátí přesnost vyhledávání.
		 * @return Přesnost vyhledávání.
		 */
		float getRecall() const;
		/**
		 * Pěkně vypíše uplynulý čas.
		 * @param[out] s Výstupní stream.
		 */
		void prettyPrintElapsed(std::ostream& s) const;
		/**
		 * Konstruktor.
		 * @param[in] efSearch @ref efSearch
		 */
		QueryBenchmark(const uint efSearch);
		/**
		 * Nastaví uplynulý čas.
		 * @param[in] elapsed Uplynulý čas.
		 */
		void setElapsed(const chr::nanoseconds& elapsed);
		/**
		 * Nastaví přesnost vyhledávání.
		 * @param[in] recall Přesnost vyhledávání.
		 */
		void setRecall(const float recall);
	};

	/**
	 * Konfigurace pro výpis tabulky
	 * závislosti parametru vyhledávání @ref Configuration::efSearch na přesnosti.
	 */
	struct RecallTableConfig {
		/**
		 * Název datového souboru bez přípony.
		 */
		std::string datasetName;
		/**
		 * Parametr stavby indexu @ref Configuration::efConstruction.
		 */
		uint efConstruction;
		/**
		 * Pole hodnot parametru vyhledávání @ref Configuration::efSearch.
		 */
		std::vector<chm::uint> efSearchValues;
		/**
		 * Šablona indexu.
		 */
		IndexTemplate indexTemplate;
		/**
		 * Parametr stavby indexu @ref Configuration::mMax.
		 */
		uint mMax;
		/**
		 * Nastavení generátoru náhodných úrovní nových prvků.
		 */
		uint seed;
		/**
		 * SIMD rozšíření použité při stavbě indexu.
		 */
		SIMDType simdType;

		/**
		 * Vrátí pole konfigurací, které obsahuje jedno JSON pole.
		 * @param[in] arr JSON pole.
		 * @param[in] configPath Cesta k původnímu JSON souboru.
		 * @return Pole konfigurací.
		 */
		static std::vector<RecallTableConfig> getVectorFromJSON(
			const nl::json& arr, const fs::path& configPath
		);
		/**
		 * Konstruktor z JSON objektu.
		 * @param[in] obj JSON objekt.
		 * @param[in] configPath Cesta k původnímu JSON souboru.
		 */
		RecallTableConfig(const nl::json& obj, const fs::path& configPath);
	};

	/**
	 * Abstraktní třída pro výpis tabulky
	 * závislosti parametru vyhledávání @ref Configuration::efSearch na přesnosti.
	 */
	struct AbstractRecallTable {
		/**
		 * Vypíše tabulku.
		 * @param[in] s Výstupní stream.
		 */
		virtual void print(std::ostream& s) const = 0;
		/**
		 * Vypočítá tabulku.
		 * @param[in] configPath Cesta k původnímu JSON souboru s konfigurací.
		 * @param[in] dataDir Cesta k adresáři s datovými soubory.
		 * @param[in] datasets JSON pole popisů datových souborů.
		 * @param[out] s Stream pro výpis tabulky.
		 */
		virtual void run(
			const fs::path& configPath, const fs::path& dataDir,
			const nl::json& datasets, std::ostream& s
		) = 0;
	};

	/**
	 * Typ sdíleného ukazatele na abstraktní třídu @ref AbstractRecallTable.
	 */
	using RecallTablePtr = std::shared_ptr<AbstractRecallTable>;

	/**
	 * Šablonová třída pro výpis tabulky
	 * závislosti parametru vyhledávání @ref Configuration::efSearch na přesnosti.
	 * @tparam T Šablona indexu. @see IndexTemplate
	 */
	template<class T = NaiveTemplate>
	class RecallTable : public AbstractRecallTable {
		/**
		 * Pole naměřených výsledků.
		 */
		std::vector<QueryBenchmark> benchmarks;
		/**
		 * Čas stavby indexu v nanosekundách.
		 */
		chr::nanoseconds buildElapsed;
		/**
		 * Konfigurace tabulky.
		 */
		const RecallTableConfig cfg;
		/**
		 * Krátký popis datového souboru.
		 */
		std::string datasetStr;
		/**
		 * Krátký popis indexu.
		 */
		std::string indexStr;

	public:
		/**
		 * Konstruktor.
		 * @param[in] cfg Konfigurace tabulky.
		 */
		RecallTable(const RecallTableConfig& cfg);
		/**
		 * Vypíše tabulku.
		 * @param[in] s Výstupní stream.
		 */
		void print(std::ostream& s) const override;
		/**
		 * Vypočítá tabulku.
		 * @param[in] configPath Cesta k původnímu JSON souboru s konfigurací.
		 * @param[in] dataDir Cesta k adresáři s datovými soubory.
		 * @param[in] datasets JSON pole popisů datových souborů.
		 * @param[out] s Stream pro výpis tabulky.
		 */
		void run(
			const fs::path& configPath, const fs::path& dataDir,
			const nl::json& datasets, std::ostream& s
		) override;
	};

	/**
	 * Získá nový objekt pro výpis tabulky
	 * závislosti parametru vyhledávání @ref Configuration::efSearch na přesnosti.
	 * Jeden typ ukazatele lze využít pro všechny šablony indexu.
	 * @param[in] cfg Konfigurace tabulky.
	 */
	RecallTablePtr getRecallTable(const RecallTableConfig& cfg);
	/**
	 * Vypíše buňku tabulky.
	 * @tparam T Primitivní datový typ.
	 * @param[in] field Hodnota buňky.
	 * @param[out] s Výstupní stream.
	 * @param[in] width Šířka buňky. Počet znaků.
	 */
	template<typename T> void printField(const T& field, std::ostream& s, const std::streamsize width);

	template<class T>
	inline RecallTable<T>::RecallTable(const RecallTableConfig& cfg)
		: buildElapsed(0), cfg(cfg), indexStr("") {

		this->benchmarks.reserve(this->cfg.efSearchValues.size());
	}

	template<class T>
	inline void RecallTable<T>::print(std::ostream& s) const {
		if (this->indexStr.empty())
			throw std::runtime_error("Recall table not yet computed.");

		std::ios streamState(nullptr);
		streamState.copyfmt(s);

		s << this->datasetStr << '\n' << this->indexStr << '\n';
		s << "Build time: [";
		prettyPrint(this->buildElapsed, s);
		s << "], " << this->buildElapsed.count() << " ns\n\n";

		printField("EfSearch", s, EF_SEARCH_WIDTH);
		printField("Recall", s, RECALL_WIDTH);
		printField("Elapsed (pretty)", s, ELAPSED_PRETTY_WIDTH);
		printField("Elapsed (ns)", s, ELAPSED_WIDTH);
		printField("\n", s, 1);

		for(const auto& benchmark : this->benchmarks) {
			printField(benchmark.efSearch, s, EF_SEARCH_WIDTH);
			s << std::right << std::setw(RECALL_WIDTH);
			chm::print(benchmark.getRecall(), s, 3);
			s << std::right << std::setw(ELAPSED_PRETTY_WIDTH);
			benchmark.prettyPrintElapsed(s);
			printField(benchmark.getElapsedNum(), s, ELAPSED_WIDTH);
			printField("\n", s, 1);
		}

		s.copyfmt(streamState);
	}

	template<class T>
	inline void RecallTable<T>::run(
		const fs::path& configPath, const fs::path& dataDir,
		const nl::json& datasets, std::ostream& s
	) {
		Timer timer{};
		this->benchmarks.clear();

		s << "Building index.\n";

		timer.reset();
		const typename Dataset<T>::Ptr dataset = Dataset<T>::getDataset(
			configPath, dataDir, datasets, this->cfg.datasetName, s
		);
		auto index = dataset->getIndex(
			this->cfg.efConstruction, this->cfg.mMax, this->cfg.seed, this->cfg.simdType
		);
		dataset->build(index);
		this->buildElapsed = timer.getElapsed();

		this->datasetStr = dataset->getString();
		this->indexStr = index->getString();

		s << "Index built in ";
		prettyPrint(this->buildElapsed, s);
		s << "\n\n";

		for(const auto& efSearch : this->cfg.efSearchValues) {
			auto& benchmark = this->benchmarks.emplace_back(efSearch);

			s << "Querying with efSearch = " << efSearch << ".\n";

			timer.reset();
			const auto knnResults = dataset->query(index, efSearch);
			benchmark.setElapsed(timer.getElapsed());

			s << "Completed in ";
			benchmark.prettyPrintElapsed(s);
			s << "\nComputing recall.\n";

			timer.reset();
			benchmark.setRecall(dataset->getRecall(knnResults));
			const auto recallElapsed = timer.getElapsed();
			s << "Recall " << benchmark.getRecall() << " computed in ";
			prettyPrint(recallElapsed, s);
			s << "\n\n";
		}
	}

	template<typename T>
	inline void printField(const T& field, std::ostream& s, const std::streamsize width) {
		s << std::right << std::setw(width) << field;
	}
}
