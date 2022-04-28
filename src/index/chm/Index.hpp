#pragma once
#include <memory>
#include <sstream>
#include "Configuration.hpp"
#include "Connections.hpp"
#include "HeapPair.hpp"
#include "IndexTemplate.hpp"
#include "LevelGenerator.hpp"
#include "KnnResults.hpp"
#include "Space.hpp"
#include "VisitedSet.hpp"

namespace chm {
	/**
	 * Hlavní třída indexu HNSW, která využívá všech ostatních tříd.
	 * @tparam T Třída určující metodu výběru sousedů, typ seznamu navštívených vrcholů
	 * a druh přístupu do paměti při výpočtu vzdálenosti. @see IndexTemplate.
	 */
	template<class T = NaiveTemplate>
	class Index {
		/**
		 * Konfigurace indexu.
		 */
		Configuration cfg;
		/**
		 * Seznamy sousedů.
		 */
		Connections conn;
		/**
		 * Identita vstupního prvku.
		 */
		uint entryID;
		/**
		 * Úroveň vstupního prvku.
		 */
		uint entryLevel;
		/**
		 * Aktuální počáteční vrchol pro procházení následující vrstvy.
		 */
		Node ep;
		/**
		 * Generátor náhodné úrovně nových prvků.
		 */
		LevelGenerator gen;
		/**
		 * Dvojice hald pro seřazování vrcholů.
		 */
		HeapPair heaps;
		/**
		 * Objekt, který ukládá vektory prvků a metriku vzdálenosti.
		 */
		Space space;
		/**
		 * Seznam navštívených vrcholů.
		 */
		typename T::VisitedSet visited;

		/**
		 * Naplní haldu vrcholy z právě upravovaného seznamu sousedů
		 * a přidá do ní nově příchozí prvek.
		 * @param[in] h Halda, která se má naplnit.
		 * @param[in] N Právě upravovaný seznam sousedů.
		 * @param[in] queryID Identita prvku, kterému index právě upravuje seznam sousedů.
		 * @param[in] latestData Vektor nově příchozího prvku.
		 * @param[in] latestID Identita nově příchozího prvku.
		 */
		template<class Comparator>
		void fillHeap(
			Heap<Comparator>& h, const Neighbors& N, const uint queryID,
			const float* const latestData, const uint latestID
		);
		/**
		 * Funkce jedné iterace stavby, která vloží nový prvek do indexu.
		 * @param[in] queryData Vektor nově příchozího prvku.
		 * @param[in] queryID Identita nově příchozího prvku.
		 */
		void insert(const float* const queryData, const uint queryID);
		/**
		 * Rozhodne o přidání prvku do haldy dosud nalezených sousedů při zpracování nižší vrstvy.
		 * @param[in] neighborID Identita prvku, o kterém se má rozhodnout.
		 * @param[in] query Vektor prvku, kterému index právě hledá sousedy.
		 * @param[in] ef Maximální počet výsledných sousedů.
		 * @param[in,out] W Halda dosud nalezených sousedů.
		 */
		void processNeighbor(
			const uint neighborID, const float* const query, const uint ef, FarHeap& W
		);
		/**
		 * Vloží kolekci prvků do indexu.
		 * @param[in] arr Kolekce prvků.
		 */
		void push(const FloatArray& arr);
		/**
		 * Zpracuje kolekci dotazů.
		 * @param[in] arr Kolekce dotazů.
		 * @param[in] k Počet hledaných sousedů pro každý dotaz.
		 * @return Výsledek hledání.
		 */
		KnnResults queryBatch(const FloatArray& arr, const uint k);
		/**
		 * Zpracuje jeden dotaz.
		 * @param[in] data Vektor dotazovaného prvku.
		 * @param[in] k Počet hledaných sousedů dotazovaného prvku.
		 * @return Halda s výslednými sousedy.
		 */
		FarHeap queryOne(const float* const data, const uint k);
		/**
		 * Nastaví počáteční vrchol @ref ep na vstupní prvek se vzdáleností od zkoumaného prvku.
		 * @param[in] query Vektor zkoumaného prvku.
		 */
		void resetEp(const float* const query);
		/**
		 * Najde sousedy zkoumaného prvku na nižší vrstvě a zapíše je do haldy @ref far.
		 * @tparam searching Pravda, pokud je tato metoda volána z metody @ref queryOne.
		 * @param[in] query Vektor zkoumaného prvku.
		 * @param[in] ef Maximální počet výsledných sousedů.
		 * @param[in] lc Číselné označení vrstvy.
		 * @param[in] countBeforeQuery Počet prvků v indexu s ukončenou iterací stavby.
		 */
		template<bool searching>
		void searchLowerLayer(
			const float* const query, const uint ef, const uint lc, const uint countBeforeQuery
		);
		/**
		 * Nalezne lokální minimum ke zkoumanému prvku na vyšší vrstvě.
		 * @param[in] query Vektor zkoumaného prvku.
		 * @param[in] lc Číselné označení vrstvy.
		 */
		void searchUpperLayer(const float* const query, const uint lc);
		/**
		 * Vybere sousedy nového prvku na dané vrstvě
		 * a vytvoří hrany vedoucí od nového prvku k těmto sousedům.
		 * Metodu výběru sousedů zvolí na základě šablony @p T.
		 * @param[in] queryID Identita nového prvku.
		 * @param[in] lc Číselné označení vrstvy.
		 * @return Seznam vybraných sousedů.
		 */
		Neighbors selectNewNeighbors(const uint queryID, const uint lc);
		/**
		 * Vybere sousedy nového prvku na dané vrstvě
		 * a vytvoří hrany vedoucí od nového prvku k těmto sousedům.
		 * Pro výběr sousedů využívá heuristiku.
		 * @param[in,out] R Seznam vybraných sousedů.
		 * @return Seznam vybraných sousedů.
		 */
		Neighbors selectNewNeighborsHeuristic(Neighbors& R);
		/**
		 * Vybere sousedy nového prvku a vytvoří hrany vedoucí od nového prvku k těmto sousedům.
		 * Pro výběr sousedů využívá naivní algoritmus.
		 * @param[in,out] N Seznam vybraných sousedů.
		 * @return Seznam vybraných sousedů.
		 */
		Neighbors selectNewNeighborsNaive(Neighbors& N);
		/**
		 * Upraví seznam sousedů tak, aby jeho velikost nepřekračovala povolenou mez.
		 * Metodu výběru sousedů zvolí na základě šablony @p T.
		 * @param[in] M Maximální počet sousedů v seznamu.
		 * @param[in] queryID Identita prvku, kterému patří seznam sousedů.
		 * @param[in,out] R Seznam sousedů.
		 * @param[in] latestData Vektor nově příchozího prvku.
		 * @param[in] latestID Identita nově příchozího prvku.
		 */
		void shrinkNeighbors(
			const uint M, const uint queryID, Neighbors& R,
			const float* const latestData, const uint latestID
		);
		/**
		 * Upraví seznam sousedů tak, aby jeho velikost nepřekračovala povolenou mez.
		 * Pro výběr sousedů využívá heuristiku.
		 * @param[in] M Maximální počet sousedů v seznamu.
		 * @param[in] queryID Identita prvku, kterému patří seznam sousedů.
		 * @param[in,out] R Seznam sousedů.
		 * @param[in] latestData Vektor nově příchozího prvku.
		 * @param[in] latestID Identita nově příchozího prvku.
		 */
		void shrinkNeighborsHeuristic(
			const uint M, const uint queryID, Neighbors& R,
			const float* const latestData, const uint latestID
		);
		/**
		 * Upraví seznam sousedů tak, aby jeho velikost nepřekračovala povolenou mez.
		 * Pro výběr sousedů využívá naivní algoritmus.
		 * @param[in] M Maximální počet sousedů v seznamu.
		 * @param[in] queryID Identita prvku, kterému patří seznam sousedů.
		 * @param[in,out] R Seznam sousedů.
		 * @param[in] latestData Vektor nově příchozího prvku.
		 * @param[in] latestID Identita nově příchozího prvku.
		 */
		void shrinkNeighborsNaive(
			const uint M, const uint queryID, Neighbors& R,
			const float* const latestData, const uint latestID
		);
		/**
		 * Vrátí hodnotu pravda, pokud seznam navštívených vrcholů visited využívá bitového pole.
		 * @return Vlajka využití bitového pole.
		 */
		static constexpr bool useBitArray();

	public:
		/**
		 * Vrátí krátký popis indexu.
		 * @return Popis indexu.
		 */
		std::string getString() const;
		/**
		 * Konstruktor.
		 * @param[in] dim Počet dimenzí prostoru.
		 * @param[in] maxCount Maximální počet prvků v indexu.
		 * @param[in] efConstruction Počet prvků,
		 * ze kterých index vybírá nové sousedy při vkládání nového prvku. @see Configuration::efConstruction
		 * @param[in] mMax Maximální počet sousedů prvku na vrstvách vyšších než vrstva 0.
		 * @see Configuration::mMax
		 * @param[in] seed Konfigurace generátoru náhodných úrovní nových prvků.
		 * @see LevelGenerator
		 * @param[in] simdType Druh SIMD instrukcí použitých při výpočtu vzdáleností.
		 * @param[in] Druh prostoru dle metriky.
		 */
		Index(
			const size_t dim, const uint maxCount,
			const uint efConstruction = 200, const uint mMax = 16, const uint seed = 100,
			const SIMDType simdType = SIMDType::NONE, const SpaceKind spaceKind = SpaceKind::EUCLIDEAN
		);
		/**
		 * Vloží kolekci prvků do indexu.
		 * @param[in] data Ukazatel na vektor prvního prvku v kolekci.
		 * @param[in] count Počet prvků v kolekci.
		 */
		void push(const float* const data, const uint count);
		/**
		 * Zpracuje kolekci dotazů.
		 * @param[in] data Ukazatel na vektor prvního dotazu v kolekci.
		 * @param[in] count Počet dotazů v kolekci.
		 * @param[in] k Počet hledaných sousedů pro každý dotaz.
		 * @return Kolekce výsledných sousedů.
		 */
		KnnResults queryBatch(const float* const data, const uint count, const uint k = 10);
		/**
		 * Nastaví parametr vyhledávání efSearch.
		 * @param[in] efSearch Parametr vyhledávání.
		 * @see Configuration::efSearch
		 */
		void setEfSearch(const uint efSearch);

		#ifdef PYBIND_INCLUDED

			/**
			 * Vloží kolekci prvků popsanou pomocí NumPy pole do indexu.
			 * @param[in] data NumPy pole vektorů prvků.
			 */
			void push(const NumpyArray<float> data);
			/**
			 * Zpracuje kolekci dotazů popsanou pomocí NumPy pole.
			 * @param[in] data NumPy pole vektorů dotazů.
			 * @param[in] k Počet hledaných sousedů pro každý dotaz.
			 * @return Uspořádaná dvojice (identity, vzdálenosti) výsledných sousedů.
			 */
			py::tuple queryBatch(const NumpyArray<float> data, const uint k = 10);

		#endif
	};

	/**
	 * Typ sdíleného ukazatele na index.
	 */
	template<class T>
	using IndexPtr = std::shared_ptr<Index<T>>;

	template<class T>
	template<class Comparator>
	inline void Index<T>::fillHeap(
		Heap<Comparator>& h, const Neighbors& N, const uint queryID,
		const float* const latestData, const uint latestID
	) {
		const auto data = this->space.getData(queryID);
		h.clear();
		h.push(this->space.getDistance(data, latestData), latestID);

		if constexpr(T::usePrefetch) {
			const auto lastIdx = N.len() - 1;
			this->space.prefetch(N.get(0));

			for(size_t i = 0; i < lastIdx; i++) {
				this->space.prefetch(N.get(i + 1));

				const auto& id = N.get(i);
				h.push(this->space.getDistance(data, id), id);
			}

			const auto& id = N.get(lastIdx);
			h.push(this->space.getDistance(data, id), id);

		} else
			for(const auto& id : N)
				h.push(this->space.getDistance(data, id), id);
	}

	template<class T>
	inline void Index<T>::insert(
		const float* const queryData, const uint queryID
	) {
		const auto L = this->entryLevel;
		const auto l = this->gen.getNext();

		this->conn.init(queryID, l);
		this->resetEp(queryData);

		for(auto lc = L; lc > l; lc--)
			this->searchUpperLayer(queryData, lc);

		for(auto lc = std::min(L, l);; lc--) {
			this->searchLowerLayer<false>(queryData, this->cfg.efConstruction, lc, queryID);
			const auto W = this->selectNewNeighbors(queryID, lc);

			const auto mLayer = !lc ? this->cfg.mMax0 : this->cfg.mMax;

			for(const auto& id : W) {
				auto N = this->conn.getNeighbors(id, lc);

				if(N.len() < mLayer)
					N.push(queryID);
				else
					this->shrinkNeighbors(mLayer, id, N, queryData, queryID);
			}

			if(!lc)
				break;
		}

		if(l > L) {
			this->entryID = queryID;
			this->entryLevel = l;
		}
	}

	template<class T>
	inline void Index<T>::processNeighbor(
		const uint neighborID, const float* const query, const uint ef, FarHeap& W
	) {
		const auto distance = this->space.getDistance(query, neighborID);
		bool shouldAdd{};

		{
			const auto& f = W.top();
			shouldAdd = f.distance > distance || W.len() < ef;
		}

		if(shouldAdd) {
			this->heaps.push(distance, neighborID);

			if(W.len() > ef)
				W.pop();
		}
	}

	template<class T>
	inline void Index<T>::push(const FloatArray& arr) {
		uint i = 0;

		if(this->space.isEmpty()) {
			i++;
			this->entryLevel = this->gen.getNext();
			this->conn.init(this->entryID, this->entryLevel);
		}

		const auto prevCount = this->space.getCount();
		this->space.push(arr.data, arr.count);

		for(; i < arr.count; i++) {
			const auto queryID = prevCount + i;
			this->insert(this->space.getData(queryID), queryID);
		}
	}

	template<class T>
	inline KnnResults Index<T>::queryBatch(const FloatArray& arr, const uint k) {
		KnnResults res(arr.count, k);

		for(size_t queryIdx = 0; queryIdx < arr.count; queryIdx++) {
			auto heap = this->queryOne(
				this->space.getNormalizedQuery(arr.data + queryIdx * this->space.dim), k
			);

			for(auto neighborIdx = k - 1;; neighborIdx--) {
				{
					const auto& node = heap.top();
					res.setData(queryIdx, neighborIdx, node.distance, node.id);
				}
				heap.pop();

				if(!neighborIdx)
					break;
			}
		}

		return res;
	}

	template<class T>
	inline FarHeap Index<T>::queryOne(const float* const data, const uint k) {
		const auto maxEf = this->cfg.getMaxEf(k);
		this->heaps.reserve(std::max(maxEf, this->cfg.mMax0));
		this->resetEp(data);
		const auto L = this->entryLevel;

		for(auto lc = L; lc > 0; lc--)
			this->searchUpperLayer(data, lc);

		this->searchLowerLayer<true>(data, maxEf, 0, this->space.getCount());

		while(this->heaps.far.len() > k)
			this->heaps.far.pop();

		return this->heaps.far;
	}

	template<class T>
	inline void Index<T>::resetEp(const float* const query) {
		this->ep.distance = this->space.getDistance(query, this->entryID);
		this->ep.id = this->entryID;
	}

	template<class T>
	template<bool searching>
	inline void Index<T>::searchLowerLayer(
		const float* const query, const uint ef, const uint lc, const uint countBeforeQuery
	) {
		this->heaps.prepareLowerSearch(this->ep);
		this->visited.prepare(countBeforeQuery, this->ep.id);
		auto& C = this->heaps.near;
		auto& W = this->heaps.far;

		while(C.len()) {
			uint cand{};

			{
				const auto& c = C.top();
				const auto& f = W.top();

				if constexpr(searching) {
					if(c.distance > f.distance)
						break;
				} else {
					if(c.distance > f.distance && W.len() == ef)
						break;
				}

				cand = c.id;
			}

			// Extract nearest from C.
			C.pop();
			const auto N = this->conn.getNeighbors(cand, lc);

			if constexpr(T::usePrefetch) {
				if constexpr(!Index<T>::useBitArray()) {
					if(!N.len())
						continue;

					this->visited.prefetch(N.get(0));
				}

				VisitResult visRes = this->visited.insertNext(N, 0);

				if(visRes.success)
					this->space.prefetch(visRes.neighborID);

				for(;;) {
					if(!visRes.success)
						break;

					const auto currID = visRes.neighborID;

					visRes = this->visited.insertNext(N, visRes.idx + 1);

					if(visRes.success)
						this->space.prefetch(visRes.neighborID);

					this->processNeighbor(currID, query, ef, W);
				}

			} else
				for(const auto& id : N)
					if(this->visited.insert(id))
						this->processNeighbor(id, query, ef, W);
		}
	}

	template<class T>
	inline void Index<T>::searchUpperLayer(
		const float* const query, const uint lc
	) {
		uint prev{};

		do {
			const auto N = this->conn.getNeighbors(this->ep.id, lc);
			prev = this->ep.id;

			if constexpr(T::usePrefetch) {
				const auto len = N.len();

				if(!len)
					continue;

				const auto lastIdx = len - 1;
				this->space.prefetch(N.get(0));

				for(size_t i = 0; i < lastIdx; i++) {
					this->space.prefetch(N.get(i + 1));

					const auto cand = N.get(i);
					const auto distance = this->space.getDistance(query, cand);

					if(distance < this->ep.distance) {
						this->ep.distance = distance;
						this->ep.id = cand;
					}
				}

				const auto cand = N.get(lastIdx);
				const auto distance = this->space.getDistance(query, cand);

				if(distance < this->ep.distance) {
					this->ep.distance = distance;
					this->ep.id = cand;
				}

			} else {
				for(const auto& cand : N) {
					const auto distance = this->space.getDistance(query, cand);

					if(distance < this->ep.distance) {
						this->ep.distance = distance;
						this->ep.id = cand;
					}
				}
			}
		} while(this->ep.id != prev);
	}

	template<class T>
	inline Neighbors Index<T>::selectNewNeighbors(
		const uint queryID, const uint lc
	) {
		auto N = this->conn.getNeighbors(queryID, lc);

		if constexpr(T::useHeuristic)
			return this->selectNewNeighborsHeuristic(N);
		return this->selectNewNeighborsNaive(N);
	}

	template<class T>
	inline Neighbors Index<T>::selectNewNeighborsHeuristic(Neighbors& R) {
		if(this->heaps.far.len() <= this->cfg.mMax) {
			R.fillFrom(this->heaps.far, this->ep);
			return R;
		}

		this->heaps.prepareHeuristic();
		auto& W = this->heaps.near;

		{
			const auto& e = W.top();
			this->ep.distance = e.distance;
			this->ep.id = e.id;
			R.push(e.id);
		}

		W.pop();

		while(W.len() && R.len() < this->cfg.mMax) {
			{
				const auto& e = W.top();
				const auto eData = this->space.getData(e.id);

				if constexpr(T::usePrefetch) {
					const auto lastIdx = R.len() - 1;
					this->space.prefetch(R.get(0));

					for(size_t i = 0; i < lastIdx; i++) {
						this->space.prefetch(R.get(i + 1));

						if(this->space.getDistance(eData, R.get(i)) < e.distance)
							goto isNotCloser;
					}

					if(this->space.getDistance(eData, R.get(lastIdx)) < e.distance)
						goto isNotCloser;

				} else {
					for(const auto& rID : R)
						if(this->space.getDistance(eData, rID) < e.distance)
							goto isNotCloser;
				}

				R.push(e.id);

				if(e.distance < this->ep.distance) {
					this->ep.distance = e.distance;
					this->ep.id = e.id;
				}
			}

			isNotCloser:;

			// Extract nearest from W.
			W.pop();
		}

		return R;
	}

	template<class T>
	inline Neighbors Index<T>::selectNewNeighborsNaive(Neighbors& N) {
		auto& W = this->heaps.far;

		if(W.len() > this->cfg.mMax)
			while(W.len() > this->cfg.mMax)
				W.pop();

		N.fillFrom(W, this->ep);
		return N;
	}

	template<class T>
	inline void Index<T>::shrinkNeighbors(
		const uint M, const uint queryID, Neighbors& R,
		const float* const latestData, const uint latestID
	) {
		if constexpr(T::useHeuristic)
			this->shrinkNeighborsHeuristic(M, queryID, R, latestData, latestID);
		else
			this->shrinkNeighborsNaive(M, queryID, R, latestData, latestID);
	}

	template<class T>
	inline void Index<T>::shrinkNeighborsHeuristic(
		const uint M, const uint queryID, Neighbors& R,
		const float* const latestData, const uint latestID
	) {
		this->fillHeap(this->heaps.near, R, queryID, latestData, latestID);

		auto& W = this->heaps.near;
		R.clear();
		R.push(W.top().id);
		W.pop();

		while(W.len() && R.len() < this->cfg.mMax) {
			{
				const auto& e = W.top();
				const auto eData = this->space.getData(e.id);

				if constexpr(T::usePrefetch) {
					const auto lastIdx = R.len() - 1;
					this->space.prefetch(R.get(0));

					for(size_t i = 0; i < lastIdx; i++) {
						this->space.prefetch(R.get(i + 1));

						if(this->space.getDistance(eData, R.get(i)) < e.distance)
							goto isNotCloser;
					}

					if(this->space.getDistance(eData, R.get(lastIdx)) < e.distance)
						goto isNotCloser;

				} else {
					for(const auto& rID : R)
						if(this->space.getDistance(eData, rID) < e.distance)
							goto isNotCloser;
				}

				R.push(e.id);
			}

			isNotCloser:;

			// Extract nearest from W.
			W.pop();
		}
	}

	template<class T>
	inline void Index<T>::shrinkNeighborsNaive(
		const uint M, const uint queryID, Neighbors& R,
		const float* const latestData, const uint latestID
	) {
		auto& W = this->heaps.far;
		this->fillHeap(W, R, queryID, latestData, latestID);

		while(W.len() > M)
			W.pop();

		R.fillFrom(W);
	}

	template<class T>
	inline constexpr bool Index<T>::useBitArray() {
		return std::is_same<typename T::VisitedSet, VisitedSet<bool>>::value;
	}

	template<class T>
	inline std::string Index<T>::getString() const {
		std::stringstream s;
		/*
			Význam zkratek v popisu indexu.
			e ... efConstruction
			m ... mMax
			d ... Název funkce metriky.
			b ... Index využívá bitového pole jako seznam navštívených prvků (0 = Ne, 1 = Ano).
			n ... Metoda výběru sousedů (h = Heuristika, n = Naivní algoritmus).
			p ... Index využívá asynchronního přístupu do paměti při výpočtu vzdáleností (0 = Ne, 1 = Ano).
		*/
		s << "chm(e=" << this->cfg.efConstruction << ",m=" << this->cfg.mMax <<
		",d=" << this->space.getDistanceName() << ",b=";

		if constexpr(Index<T>::useBitArray())
			s << "1";
		else
			s << "0";

		s << ",n=";

		if constexpr(T::useHeuristic)
			s << 'h';
		else
			s << 'n';

		s << ",p=";

		if constexpr(T::usePrefetch)
			s << '1';
		else
			s << '0';

		s << ')';

		return s.str();
	}

	template<class T>
	inline Index<T>::Index(
		const size_t dim, const uint maxCount, const uint efConstruction, const uint mMax,
		const uint seed, const SIMDType simdType, const SpaceKind spaceKind
	) : cfg(efConstruction, mMax), conn(maxCount, this->cfg.mMax, this->cfg.mMax0),
		entryID(0), entryLevel(0), ep{}, gen(this->cfg.getML(), seed),
		heaps(efConstruction, this->cfg.mMax), space(dim, spaceKind, maxCount, simdType),
		visited(maxCount) {}

	template<class T>
	inline void Index<T>::push(const float* const data, const uint count) {
		this->push(FloatArray(data, count));
	}

	template<class T>
	inline KnnResults Index<T>::queryBatch(
		const float* const data, const uint count, const uint k
	) {
		return this->queryBatch(FloatArray(data, count), k);
	}

	template<class T>
	inline void Index<T>::setEfSearch(const uint efSearch) {
		this->cfg.setEfSearch(efSearch);
	}

	#ifdef PYBIND_INCLUDED

		template<class T>
		inline void Index<T>::push(const NumpyArray<float> data) {
			this->push(FloatArray(data, this->space.dim));
		}

		template<class T>
		inline py::tuple Index<T>::queryBatch(
			const NumpyArray<float> data, const uint k
		) {
			return this->queryBatch(FloatArray(data, this->space.dim), k).makeTuple();
		}

	#endif
}
