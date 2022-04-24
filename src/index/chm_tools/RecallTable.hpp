#pragma once
#include <chrono>
#include "Dataset.hpp"
#include "libs/json.hpp"

namespace chm {
	namespace chr = std::chrono;
	namespace fs = std::filesystem;
	namespace nl = nlohmann;

	constexpr std::streamsize EF_SEARCH_WIDTH = 8;
	constexpr std::streamsize ELAPSED_PRETTY_WIDTH = 29;
	constexpr std::streamsize ELAPSED_WIDTH = 29;
	constexpr std::streamsize RECALL_WIDTH = 12;

	class QueryBenchmark {
		chr::nanoseconds elapsed;
		float recall;

	public:
		const uint efSearch;

		long long getElapsedNum() const;
		float getRecall() const;
		void prettyPrintElapsed(std::ostream& s) const;
		QueryBenchmark(const uint efSearch);
		void setElapsed(const chr::nanoseconds& elapsed);
		void setRecall(const float recall);
	};

	struct RecallTableConfig {
		fs::path datasetPath;
		uint efConstruction;
		std::vector<chm::uint> efSearchValues;
		IndexTemplate indexTemplate;
		uint mMax;
		uint seed;
		SIMDType simdType;

		static std::vector<RecallTableConfig> getVectorFromJSON(
			const fs::path& p, const fs::path& dataDir
		);
		RecallTableConfig(const nl::json& obj, const fs::path& dataDir);
	};

	struct AbstractRecallTable {
		virtual void print(std::ostream& s) const = 0;
		virtual void run(std::ostream& s) = 0;
	};

	using RecallTablePtr = std::shared_ptr<AbstractRecallTable>;

	template<class T = NaiveTemplate>
	class RecallTable : public AbstractRecallTable {
		std::vector<QueryBenchmark> benchmarks;
		chr::nanoseconds buildElapsed;
		const RecallTableConfig cfg;
		std::string datasetStr;
		std::string indexStr;

	public:
		RecallTable(const RecallTableConfig& cfg);
		void print(std::ostream& s) const override;
		void run(std::ostream& s) override;
	};

	class Timer {
		chr::steady_clock::time_point start;

	public:
		chr::nanoseconds getElapsed() const;
		void reset();
		Timer();
	};

	template<typename T> long long convert(chr::nanoseconds& t);
	template<typename T> T getJSONValue(const nl::json& obj, const std::string& key);
	RecallTablePtr getRecallTable(const RecallTableConfig& cfg);
	void prettyPrint(const chr::nanoseconds& elapsed, std::ostream& s);
	void print(const float number, std::ostream& s, const std::streamsize places = 2);
	void print(const long long number, std::ostream& s, const std::streamsize places = 2);
	template<typename T> void printField(const T& field, std::ostream& s, const std::streamsize width);
	void throwMissingKey(const std::string& key);
	void throwWrongType(const std::string& key, const std::string& expectedType);

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
	inline void RecallTable<T>::run(std::ostream& s) {
		Timer timer{};
		this->benchmarks.clear();

		s << "Building index.\n";

		timer.reset();
		const Dataset<T> dataset(this->cfg.datasetPath);
		auto index = dataset.getIndex(
			this->cfg.efConstruction, this->cfg.mMax, this->cfg.seed, this->cfg.simdType
		);
		dataset.build(index);
		this->buildElapsed = timer.getElapsed();

		this->datasetStr = dataset.getString();
		this->indexStr = index->getString();

		s << "Index built in ";
		prettyPrint(this->buildElapsed, s);
		s << "\n\n";

		for(const auto& efSearch : this->cfg.efSearchValues) {
			auto& benchmark = this->benchmarks.emplace_back(efSearch);

			s << "Querying with efSearch = " << efSearch << ".\n";

			timer.reset();
			const auto knnResults = dataset.query(index, efSearch);
			benchmark.setElapsed(timer.getElapsed());

			s << "Completed in ";
			benchmark.prettyPrintElapsed(s);
			s << "\nComputing recall.\n";

			timer.reset();
			benchmark.setRecall(dataset.getRecall(knnResults));
			const auto recallElapsed = timer.getElapsed();
			s << "Recall " << benchmark.getRecall() << " computed in ";
			prettyPrint(recallElapsed, s);
			s << "\n\n";
		}
	}

	template<typename T>
	inline long long convert(chr::nanoseconds& t) {
		const auto res = chr::duration_cast<T>(t);
		t -= res;
		return res.count();
	}

	template<typename T>
	inline T getJSONValue(const nl::json& obj, const std::string& key) {
		if(!obj.contains(key))
			throwMissingKey(key);

		const auto& value = obj.at(key);

		try {
			return value.get<T>();
		} catch(const nl::json::exception&) {
			throwWrongType(key, typeid(T).name());
		}
		return T();
	}

	template<typename T>
	inline void printField(const T& field, std::ostream& s, const std::streamsize width) {
		s << std::right << std::setw(width) << field;
	}
}
