#pragma once
#include <memory>
#include <sstream>
#include "Configuration.hpp"
#include "Connections.hpp"
#include "HeapPair.hpp"
#include "LevelGenerator.hpp"
#include "KnnResults.hpp"
#include "Space.hpp"
#include "VisitedSet.hpp"

namespace chm {
	template<bool useHeuristic = false, bool usePrefetch = false>
	class Index {
		Configuration cfg;
		Connections conn;
		uint entryID;
		uint entryLevel;
		Node ep;
		LevelGenerator gen;
		HeapPair heaps;
		Space space;
		VisitedSet visited;

		template<class Comparator>
		void fillHeap(
			Heap<Comparator>& h, const Neighbors& N, const uint queryID,
			const float* const latestData, const uint latestID
		);

		void insert(const float* const queryData, const uint queryID);
		void processNeighbor(
			const uint neighborID, const float* const query, const uint ef, FarHeap& W
		);
		void push(const FloatArray& arr);
		FarHeap query(const float* const data, const uint k);
		KnnResults query(const FloatArray& arr, const uint k);
		void resetEp(const float* const query);

		template<bool searching>
		void searchLowerLayer(
			const float* const query, const uint ef, const uint lc, const uint countBeforeQuery
		);

		void searchUpperLayer(const float* const query, const uint lc);
		Neighbors selectNewNeighbors(const uint queryID, const uint lc);
		Neighbors selectNewNeighborsHeuristic(Neighbors& R);
		Neighbors selectNewNeighborsNaive(Neighbors& N);
		void shrinkNeighbors(
			const uint M, const uint queryID, Neighbors& R,
			const float* const latestData, const uint latestID
		);
		void shrinkNeighborsHeuristic(
			const uint M, const uint queryID, Neighbors& R,
			const float* const latestData, const uint latestID
		);
		void shrinkNeighborsNaive(
			const uint M, const uint queryID, Neighbors& R,
			const float* const latestData, const uint latestID
		);

	public:
		std::string getString() const;
		Index(
			const size_t dim, const uint maxCount,
			const uint efConstruction = 200, const uint mMax = 16, const uint seed = 100,
			const SIMDType simdType = SIMDType::NONE, const SpaceKind spaceKind = SpaceKind::EUCLIDEAN
		);

		void push(const float* const data, const uint count);
		KnnResults query(const float* const data, const uint count, const uint k = 10);
		void setEfSearch(const uint efSearch);

		#ifdef PYBIND_INCLUDED

			void push(const NumpyArray<float> data);
			py::tuple query(const NumpyArray<float> data, const uint k = 10);

		#endif
	};

	template<bool useHeuristic, bool usePrefetch>
	using IndexPtr = std::shared_ptr<Index<useHeuristic, usePrefetch>>;

	template<bool useHeuristic, bool usePrefetch>
	template<class Comparator>
	inline void Index<useHeuristic, usePrefetch>::fillHeap(
		Heap<Comparator>& h, const Neighbors& N, const uint queryID,
		const float* const latestData, const uint latestID
	) {
		const auto data = this->space.getData(queryID);
		h.clear();
		h.push(this->space.getDistance(data, latestData), latestID);

		if constexpr(usePrefetch) {
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

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::insert(
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

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::processNeighbor(
		const uint neighborID, const float* const query, const uint ef, FarHeap& W
	) {
		this->visited.insert(neighborID);
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

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::push(const FloatArray& arr) {
		uint i = 0;

		if(this->space.isEmpty()) {
			i = 1;
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

	template<bool useHeuristic, bool usePrefetch>
	inline FarHeap Index<useHeuristic, usePrefetch>::query(const float* const data, const uint k) {
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

	template<bool useHeuristic, bool usePrefetch>
	inline KnnResults Index<useHeuristic, usePrefetch>::query(const FloatArray& arr, const uint k) {
		KnnResults res(arr.count, k);

		for(size_t queryIdx = 0; queryIdx < arr.count; queryIdx++) {
			auto heap = this->query(
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

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::resetEp(const float* const query) {
		this->ep.distance = this->space.getDistance(query, this->entryID);
		this->ep.id = this->entryID;
	}

	template<bool useHeuristic, bool usePrefetch>
	template<bool searching>
	inline void Index<useHeuristic, usePrefetch>::searchLowerLayer(
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

			if constexpr(usePrefetch) {
				VisitedResult visRes = this->visited.findNext(N, 0);

				if(visRes.success)
					this->space.prefetch(visRes.neighborID);

				for(;;) {
					if(!visRes.success)
						break;

					const auto currID = visRes.neighborID;

					visRes = this->visited.findNext(N, visRes.idx + 1);

					if(visRes.success)
						this->space.prefetch(visRes.neighborID);

					this->processNeighbor(currID, query, ef, W);
				}

			} else {
				for(const auto& id : N) {
					if(this->visited.insert(id)) {
						const auto distance = this->space.getDistance(query, id);
						bool shouldAdd{};

						{
							const auto& f = W.top();
							shouldAdd = f.distance > distance || W.len() < ef;
						}

						if(shouldAdd) {
							this->heaps.push(distance, id);

							if(W.len() > ef)
								W.pop();
						}
					}
				}
			}
		}
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::searchUpperLayer(
		const float* const query, const uint lc
	) {
		uint prev{};

		do {
			const auto N = this->conn.getNeighbors(this->ep.id, lc);
			prev = this->ep.id;

			if constexpr(usePrefetch) {
				const auto lastIdx = N.len() - 1;
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

	template<bool useHeuristic, bool usePrefetch>
	inline Neighbors Index<useHeuristic, usePrefetch>::selectNewNeighbors(
		const uint queryID, const uint lc
	) {
		auto N = this->conn.getNeighbors(queryID, lc);

		if constexpr(useHeuristic)
			return this->selectNewNeighborsHeuristic(N);
		return this->selectNewNeighborsNaive(N);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline Neighbors Index<useHeuristic, usePrefetch>::selectNewNeighborsHeuristic(Neighbors& R) {
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

				if constexpr(usePrefetch) {
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

	template<bool useHeuristic, bool usePrefetch>
	inline Neighbors Index<useHeuristic, usePrefetch>::selectNewNeighborsNaive(Neighbors& N) {
		auto& W = this->heaps.far;

		if(W.len() > this->cfg.mMax)
			while(W.len() > this->cfg.mMax)
				W.pop();

		N.fillFrom(W, this->ep);
		return N;
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::shrinkNeighbors(
		const uint M, const uint queryID, Neighbors& R,
		const float* const latestData, const uint latestID
	) {
		if constexpr(useHeuristic)
			this->shrinkNeighborsHeuristic(M, queryID, R, latestData, latestID);
		else
			this->shrinkNeighborsNaive(M, queryID, R, latestData, latestID);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::shrinkNeighborsHeuristic(
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

				if constexpr(usePrefetch) {
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

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::shrinkNeighborsNaive(
		const uint M, const uint queryID, Neighbors& R,
		const float* const latestData, const uint latestID
	) {
		auto& W = this->heaps.far;
		this->fillHeap(W, R, queryID, latestData, latestID);

		while(W.len() > M)
			W.pop();

		R.fillFrom(W);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline std::string Index<useHeuristic, usePrefetch>::getString() const {
		std::stringstream s;
		s << "chm(e=" << this->cfg.efConstruction << ",m=" << this->cfg.mMax <<
		",d=" << this->space.getDistanceName() << ",n=";

		if constexpr(useHeuristic)
			s << 'h';
		else
			s << 'n';

		s << ",p=";

		if constexpr(usePrefetch)
			s << '1';
		else
			s << '0';

		s << ')';

		return s.str();
	}

	template<bool useHeuristic, bool usePrefetch>
	inline Index<useHeuristic, usePrefetch>::Index(
		const size_t dim, const uint maxCount, const uint efConstruction, const uint mMax,
		const uint seed, const SIMDType simdType, const SpaceKind spaceKind
	) : cfg(efConstruction, mMax), conn(maxCount, this->cfg.mMax, this->cfg.mMax0),
		entryID(0), entryLevel(0), ep{}, gen(this->cfg.getML(), seed),
		heaps(efConstruction, this->cfg.mMax), space(dim, spaceKind, maxCount, simdType),
		visited(maxCount) {}

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::push(const float* const data, const uint count) {
		this->push(FloatArray(data, count));
	}

	template<bool useHeuristic, bool usePrefetch>
	inline KnnResults Index<useHeuristic, usePrefetch>::query(
		const float* const data, const uint count, const uint k
	) {
		return this->query(FloatArray(data, count), k);
	}

	template<bool useHeuristic, bool usePrefetch>
	inline void Index<useHeuristic, usePrefetch>::setEfSearch(const uint efSearch) {
		this->cfg.setEfSearch(efSearch);
	}

	#ifdef PYBIND_INCLUDED

		template<bool useHeuristic, bool usePrefetch>
		inline void Index<useHeuristic, usePrefetch>::push(const NumpyArray<float> data) {
			this->push(FloatArray(data, this->space.dim));
		}

		template<bool useHeuristic, bool usePrefetch>
		inline py::tuple Index<useHeuristic, usePrefetch>::query(
			const NumpyArray<float> data, const uint k
		) {
			return this->query(FloatArray(data, this->space.dim), k).makeTuple();
		}

	#endif
}
