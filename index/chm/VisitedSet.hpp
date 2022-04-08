#pragma once
#include <type_traits>
#include "DistanceFunction.hpp"
#include "Neighbors.hpp"

namespace chm {
	struct VisitResult {
		size_t idx;
		uint neighborID;
		bool success;

		static VisitResult fail();
		VisitResult(const size_t idx, const uint neighborID, const bool success);
	};

	template<typename T>
	class VisitedSet {
		std::vector<T> v;

		static constexpr bool isBitArray();

	public:
		bool insert(const uint id);
		VisitResult insertNext(const Neighbors& N, const size_t startIdx);
		void prefetch(const uint id) const;
		void prepare(const uint count, const uint epID);
		VisitedSet(const uint maxCount);
	};

	template<typename T>
	inline constexpr bool VisitedSet<T>::isBitArray() {
		return std::is_same<T, bool>::value;
	}

	template<typename T>
	inline bool VisitedSet<T>::insert(const uint id) {
		if(this->v[id])
			return false;

		this->v[id] = true;
		return true;
	}

	template<typename T>
	inline VisitResult VisitedSet<T>::insertNext(const Neighbors& N, const size_t startIdx) {
		const size_t len(N.len());

		if constexpr(VisitedSet<T>::isBitArray())
			for(size_t idx = startIdx; idx < len; idx++) {
				const auto id = N.get(idx);

				if(this->insert(id))
					return VisitResult(idx, id, true);
			}
		else {
			const auto lastIdx = len - 1;

			for(size_t idx = startIdx; idx < lastIdx; idx++) {
				this->prefetch(N.get(idx + 1));
				const auto id = N.get(idx);

				if(this->insert(id))
					return VisitResult(idx, id, true);
			}

			const auto id = N.get(lastIdx);

			if(this->insert(id))
				return VisitResult(lastIdx, id, true);
		}

		return VisitResult::fail();
	}

	template<typename T>
	inline void VisitedSet<T>::prefetch(const uint id) const {
		#if defined(SIMD_CAPABLE)
			if constexpr(!VisitedSet<T>::isBitArray())
				_mm_prefetch(reinterpret_cast<const char*>(this->v.data() + size_t(id)), _MM_HINT_T0);
		#endif
	}

	template<typename T>
	inline void VisitedSet<T>::prepare(const uint count, const uint epID) {
		std::fill(this->v.begin(), this->v.begin() + count, false);
		this->v[epID] = true;
	}

	template<typename T>
	inline VisitedSet<T>::VisitedSet(const uint maxCount) : v(maxCount, false) {}
}
