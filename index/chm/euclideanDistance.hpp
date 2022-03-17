#pragma once
#include "DistanceInfo.hpp"
#include "enhancedInstructionSet.hpp"

namespace chm {
	static float euclid(const float* node, const float* query, const size_t dim, const size_t, const size_t, const size_t) {
		auto res = 0.f;

		for(size_t i = 0; i < dim; i++) {
			const auto diff = node[i] - query[i];
			res += diff * diff;
		}

		return res;
	}

	#if defined(USE_AVX)

		static float euclid16(const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t) {
			__m256 diff, v1, v2;
			const float* end = node + dim16;
			float PORTABLE_ALIGN32 tmp[8];
			__m256 sum = _mm256_set1_ps(0);

			while (node < end) {
				v1 = _mm256_loadu_ps(node);
				node += 8;
				v2 = _mm256_loadu_ps(query);
				query += 8;
				diff = _mm256_sub_ps(v1, v2);
				sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));

				v1 = _mm256_loadu_ps(node);
				node += 8;
				v2 = _mm256_loadu_ps(query);
				query += 8;
				diff = _mm256_sub_ps(v1, v2);
				sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));
			}

			_mm256_store_ps(tmp, sum);
			return
				tmp[0] + tmp[1] + tmp[2] + tmp[3] +
				tmp[4] + tmp[5] + tmp[6] + tmp[7];
		}

	#elif defined(USE_AVX512)

		static float euclid16(const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t) {
			__m512 diff, v1, v2;
			const float* end = node + dim16;
			__m512 sum = _mm512_set1_ps(0);
			float PORTABLE_ALIGN64 tmp[16];

			while (node < end) {
				v1 = _mm512_loadu_ps(node);
				node += 16;
				v2 = _mm512_loadu_ps(query);
				query += 16;
				diff = _mm512_sub_ps(v1, v2);
				sum = _mm512_add_ps(sum, _mm512_mul_ps(diff, diff));
			}

			_mm512_store_ps(tmp, sum);
			return
				tmp[0] + tmp[1] + tmp[2] + tmp[3] +
				tmp[4] + tmp[5] + tmp[6] + tmp[7] +
				tmp[8] + tmp[9] + tmp[10] + tmp[11] +
				tmp[12] + tmp[13] + tmp[14] + tmp[15];
		}

	#elif defined(USE_SSE)

		static float euclid16(const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t) {
			__m128 diff, v1, v2;
			const float* end = node + dim16;
			__m128 sum = _mm_set1_ps(0);
			float PORTABLE_ALIGN32 tmp[8];

			while (node < end) {
				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				diff = _mm_sub_ps(v1, v2);
				sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				diff = _mm_sub_ps(v1, v2);
				sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				diff = _mm_sub_ps(v1, v2);
				sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				diff = _mm_sub_ps(v1, v2);
				sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));
			}

			_mm_store_ps(tmp, sum);
			return tmp[0] + tmp[1] + tmp[2] + tmp[3];
		}

	#endif

	#if defined(USE_ENHANCED)

		static float euclid4(const float* node, const float* query, const size_t, const size_t dim4, const size_t, const size_t) {
			__m128 diff, v1, v2;
			const float* end = node + dim4;
			__m128 sum = _mm_set1_ps(0);
			float PORTABLE_ALIGN32 tmp[8];

			while (node < end) {
				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				diff = _mm_sub_ps(v1, v2);
				sum = _mm_add_ps(sum, _mm_mul_ps(diff, diff));
			}

			_mm_store_ps(tmp, sum);
			return tmp[0] + tmp[1] + tmp[2] + tmp[3];
		}

		static float euclid4Residual(const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t dimLeft) {
			const float front = euclid4(node, query, 0, dim4, dim16, 0);
			const float back = euclid(node + dim4, query + dim4, dimLeft, 0, 0, 0);
			return front + back;
		}

		static float euclid16Residual(const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t dimLeft) {
			const float front = euclid16(node, query, 0, dim4, dim16, 0);
			const float back = euclid(node + dim16, query + dim16, dimLeft, 0, 0, 0);
			return front + back;
		}

	#endif

	DistanceInfo getEuclideanInfo(const size_t dim, const size_t dim4, const size_t dim16) {
		size_t dimLeft = 0;
		DistanceFunction f = euclid;
		std::string name = "euclid";

		#if defined(USE_ENHANCED)
			if (dim % 16 == 0) {
				f = euclid16;
				name = "euclid16";
			}
			else if (dim % 4 == 0) {
				f = euclid4;
				name = "euclid4";
			}
			else if (dim > 16) {
				dimLeft = dim - dim16;
				f = euclid16Residual;
				name = "euclid16Residual";
			}
			else if (dim > 4) {
				dimLeft = dim - dim4;
				f = euclid4Residual;
				name = "euclid4Residual";
			}
		#endif

		return DistanceInfo(dimLeft, f, name);
	}
}
