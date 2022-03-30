#include <sstream>
#include "Index.hpp"

namespace chm {
	void Index::push(const float* const data) {
		const auto L = this->entryLevel;
		const auto l = this->gen.getNext();
		const auto queryData = this->space.push(data);
		const auto queryID = this->space.getLatestID();

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

	void Index::push(const FloatArray& arr) {
		size_t i = 0;

		if(this->space.isEmpty()) {
			i = 1;
			this->entryLevel = this->gen.getNext();
			this->conn.init(this->entryID, this->entryLevel);
			this->space.push(arr.data);
		}

		for(; i < arr.count; i++)
			this->push(arr.data + i * this->space.dim);
	}

	FarHeap Index::query(const float* const data, const uint k) {
		const auto maxEf = std::max(this->cfg.getEfSearch(), k);
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

	KnnResults Index::query(const FloatArray& arr, const uint k) {
		KnnResults res(arr.count, k);

		for(size_t queryIdx = 0; queryIdx < arr.count; queryIdx++) {
			auto heap = this->query(this->space.getNormalizedQuery(arr.data + queryIdx * this->space.dim), k);

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

	void Index::resetEp(const float* const query) {
		this->ep.distance = this->space.getDistance(query, this->entryID);
		this->ep.id = this->entryID;
	}

	void Index::searchUpperLayer(const float* const query, const uint lc) {
		uint prev{};

		do {
			const auto N = this->conn.getNeighbors(this->ep.id, lc);
			prev = this->ep.id;

			for(const auto& cand : N) {
				const auto distance = this->space.getDistance(query, cand);

				if(distance < this->ep.distance) {
					this->ep.distance = distance;
					this->ep.id = cand;
				}
			}

		} while(this->ep.id != prev);
	}

	Neighbors Index::selectNewNeighbors(const uint queryID, const uint lc) {
		auto N = this->conn.getNeighbors(queryID, lc);

		if(this->cfg.useHeuristic)
			return this->selectNewNeighborsHeuristic(N);
		return this->selectNewNeighborsNaive(N);
	}

	Neighbors Index::selectNewNeighborsHeuristic(Neighbors& R) {
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

				for(const auto& rID : R)
					if(this->space.getDistance(eData, rID) < e.distance)
						goto isNotCloser;

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

	Neighbors Index::selectNewNeighborsNaive(Neighbors& N) {
		auto& W = this->heaps.far;

		if(W.len() > this->cfg.mMax)
			while(W.len() > this->cfg.mMax)
				W.pop();

		N.fillFrom(W, this->ep);
		return N;
	}

	void Index::shrinkNeighbors(const uint M, const uint queryID, Neighbors& R, const float* const latestData, const uint latestID) {
		if(this->cfg.useHeuristic)
			this->shrinkNeighborsHeuristic(M, queryID, R, latestData, latestID);
		else
			this->shrinkNeighborsNaive(M, queryID, R, latestData, latestID);
	}

	void Index::shrinkNeighborsHeuristic(const uint M, const uint queryID, Neighbors& R, const float* const latestData, const uint latestID) {
		this->fillHeap(this->heaps.near, R, queryID, latestData, latestID);

		auto& W = this->heaps.near;
		R.clear();
		R.push(W.top().id);
		W.pop();

		while(W.len() && R.len() < this->cfg.mMax) {
			{
				const auto& e = W.top();
				const auto eData = this->space.getData(e.id);

				for(const auto& rID : R)
					if(this->space.getDistance(eData, rID) < e.distance)
						goto isNotCloser;

				R.push(e.id);
			}

			isNotCloser:;

			// Extract nearest from W.
			W.pop();
		}
	}

	void Index::shrinkNeighborsNaive(const uint M, const uint queryID, Neighbors& R, const float* const latestData, const uint latestID) {
		auto& W = this->heaps.far;
		this->fillHeap(W, R, queryID, latestData, latestID);

		while(W.len() > M)
			W.pop();

		R.fillFrom(W);
	}

	std::string Index::getString() const {
		std::stringstream s;
		s << "chm_hnsw.Index(efConstruction=" << this->cfg.efConstruction <<
			", mMax=" << this->cfg.mMax << ", distance=" << this->space.getDistanceName() <<
			", selectNeighbors=" << (this->cfg.useHeuristic ? "heuristic" : "naive") << ')';
		return s.str();
	}

	Index::Index(
		const size_t dim, const uint efConstruction, const uint maxCount,
		const uint mMax, const uint seed, const SpaceKind spaceKind, const bool useHeuristic
	) : cfg(efConstruction, mMax, useHeuristic), conn(maxCount, this->cfg.mMax, this->cfg.mMax0),
		entryID(0), entryLevel(0), ep{}, gen(this->cfg.getML(), seed),
		heaps(efConstruction, this->cfg.mMax), space(dim, spaceKind, maxCount), visited(maxCount) {}

	void Index::push(const float* const data, const uint count) {
		this->push(FloatArray(data, count));
	}

	KnnResults Index::query(const float* const data, const uint count, const uint k) {
		return this->query(FloatArray(data, count), k);
	}

	void Index::setEfSearch(const uint efSearch) {
		this->cfg.setEfSearch(efSearch);
	}

	#ifdef PYBIND_INCLUDED

		void Index::push(const NumpyArray<float> data) {
			this->push(FloatArray(data, this->space.dim));
		}

		py::tuple Index::query(const NumpyArray<float> data, const uint k) {
			return this->query(FloatArray(data, this->space.dim), k).makeTuple();
		}

	#endif
}
