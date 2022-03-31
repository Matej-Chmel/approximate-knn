#pragma once
#include <memory>
#include "Configuration.hpp"
#include "Connections.hpp"
#include "HeapPair.hpp"
#include "LevelGenerator.hpp"
#include "KnnResults.hpp"
#include "Space.hpp"
#include "VisitedSet.hpp"

namespace chm {
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
		void fillHeap(Heap<Comparator>& h, const Neighbors& N, const uint queryID, const float* const latestData, const uint latestID);

		void insert(const float* const queryData, const uint queryID);
		void processNeighbor(const uint neighborID, const float* const query, const uint ef, FarHeap& W);
		void push(const FloatArray& arr);
		FarHeap query(const float* const data, const uint k);
		KnnResults query(const FloatArray& arr, const uint k);
		void resetEp(const float* const query);

		template<bool searching>
		void searchLowerLayer(const float* const query, const uint ef, const uint lc, const uint countBeforeQuery);

		void searchUpperLayer(const float* const query, const uint lc);
		Neighbors selectNewNeighbors(const uint queryID, const uint lc);
		Neighbors selectNewNeighborsHeuristic(Neighbors& R);
		Neighbors selectNewNeighborsNaive(Neighbors& N);
		void shrinkNeighbors(const uint M, const uint queryID, Neighbors& R, const float* const latestData, const uint latestID);
		void shrinkNeighborsHeuristic(const uint M, const uint queryID, Neighbors& R, const float* const latestData, const uint latestID);
		void shrinkNeighborsNaive(const uint M, const uint queryID, Neighbors& R, const float* const latestData, const uint latestID);

	public:
		std::string getString() const;
		Index(
			const size_t dim, const uint efConstruction, const uint maxCount,
			const uint mMax, const uint seed, const SpaceKind spaceKind, const bool useHeuristic
		);

		void push(const float* const data, const uint count);
		KnnResults query(const float* const data, const uint count, const uint k);
		void setEfSearch(const uint efSearch);

		#ifdef PYBIND_INCLUDED

			void push(const NumpyArray<float> data);
			py::tuple query(const NumpyArray<float> data, const uint k);

		#endif
	};

	using IndexPtr = std::shared_ptr<Index>;

	template<class Comparator>
	inline void Index::fillHeap(Heap<Comparator>& h, const Neighbors& N, const uint queryID, const float* const latestData, const uint latestID) {
		const auto data = this->space.getData(queryID);
		h.clear();
		h.push(this->space.getDistance(data, latestData), latestID);

		for(const auto& id : N)
			h.push(this->space.getDistance(data, id), id);
	}

	template<bool searching>
	inline void Index::searchLowerLayer(
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

			FindResult findRes = this->visited.findNext(N, 0);

			for(;;) {
				if(!findRes.success)
					break;

				const auto currID = findRes.neighborID;

				findRes = this->visited.findNext(N, findRes.idx + 1);

				if(findRes.success)
					this->space.prefetch(findRes.neighborID);

				this->processNeighbor(currID, query, ef, W);
			}
		}
	}
}
