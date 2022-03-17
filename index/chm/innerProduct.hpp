#pragma once
#include "DistanceInfo.hpp"
#include "enhancedInstructionSet.hpp"

namespace chm {
	static float innerProdSum(const float* node, const float* query, const size_t dim, const size_t, const size_t, const size_t) {
		auto res = 0.f;

		for(size_t i = 0; i < dim; i++)
			res += node[i] * query[i];

		return res;
	}

	static float innerProduct(const float* node, const float* query, const size_t dim, const size_t, const size_t, const size_t) {
		return 1.f - innerProdSum(node, query, dim, 0, 0, 0);
	}

	#if defined(USE_AVX) || defined(USE_AVX512)

		static float innerProdSum4(
			const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t
		) {
			const float* end4 = node + dim4;
			const float* end16 = node + dim16;
			__m256 sum = _mm256_set1_ps(0);
			float PORTABLE_ALIGN32 tmp[8];

			while(node < end16) {
				__m256 v1 = _mm256_loadu_ps(node);
				node += 8;
				__m256 v2 = _mm256_loadu_ps(query);
				query += 8;
				sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));

				v1 = _mm256_loadu_ps(node);
				node += 8;
				v2 = _mm256_loadu_ps(query);
				query += 8;
				sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));
			}

			__m128 v1, v2;
			__m128 prod = _mm_add_ps(_mm256_extractf128_ps(sum, 0), _mm256_extractf128_ps(sum, 1));

			while(node < end4) {
				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				prod = _mm_add_ps(prod, _mm_mul_ps(v1, v2));
			}

			_mm_store_ps(tmp, prod);
			return tmp[0] + tmp[1] + tmp[2] + tmp[3];
		}

	#elif defined(USE_SSE)

		static float innerProdSum4(
			const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t
		) {
			const float* end4 = node + dim4;
			const float* end16 = node + dim16;
			float PORTABLE_ALIGN32 tmp[8];
			__m128 sum = _mm_set1_ps(0);
			__m128 v1, v2;

			while(node < end16) {
				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));
			}

			while(node < end4) {
				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));
			}

			_mm_store_ps(tmp, sum);
			return tmp[0] + tmp[1] + tmp[2] + tmp[3];
		}

	#endif

	#if defined(USE_AVX)
		static float innerProdSum16(
			const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t
		) {
			const float* end = node + dim16;
			__m256 sum = _mm256_set1_ps(0);
			float PORTABLE_ALIGN32 tmp[8];

			while(node < end) {
				__m256 v1 = _mm256_loadu_ps(node);
				node += 8;
				__m256 v2 = _mm256_loadu_ps(query);
				query += 8;
				sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));

				v1 = _mm256_loadu_ps(node);
				node += 8;
				v2 = _mm256_loadu_ps(query);
				query += 8;
				sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));
			}

			_mm256_store_ps(tmp, sum);
			return
				tmp[0] + tmp[1] + tmp[2] + tmp[3] +
				tmp[4] + tmp[5] + tmp[6] + tmp[7];
		}

	#elif defined(USE_AVX512)
		static float innerProdSum16(
			const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t
		) {
			const float* end = node + dim16;
			__m512 sum = _mm512_set1_ps(0);
			float PORTABLE_ALIGN64 tmp[16];

			while(node < end) {
				__m512 v1 = _mm512_loadu_ps(node);
				node += 16;
				__m512 v2 = _mm512_loadu_ps(query);
				query += 16;
				sum = _mm512_add_ps(sum, _mm512_mul_ps(v1, v2));
			}

			_mm512_store_ps(tmp, sum);
			return
				tmp[0] + tmp[1] + tmp[2] + tmp[3] +
				tmp[4] + tmp[5] + tmp[6] + tmp[7] +
				tmp[8] + tmp[9] + tmp[10] + tmp[11] +
				tmp[12] + tmp[13] + tmp[14] + tmp[15];
		}

	#elif defined(USE_SSE)

		static float innerProdSum16(
			const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t
		) {
			const float* end = node + dim16;
			float PORTABLE_ALIGN32 tmp[8];
			__m128 sum = _mm_set1_ps(0);
			__m128 v1, v2;

			while(node < end) {
				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

				v1 = _mm_loadu_ps(node);
				node += 4;
				v2 = _mm_loadu_ps(query);
				query += 4;
				sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));
			}

			_mm_store_ps(tmp, sum);
			return tmp[0] + tmp[1] + tmp[2] + tmp[3];
		}

	#endif

	#if defined(USE_ENHANCED)

		static float innerProduct4(
			const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t
		) {
			return 1.f - innerProdSum4(node, query, 0, dim4, dim16, 0);
		}

		static float innerProduct4Residual(
			const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t dimLeft
		) {
			const float front = innerProdSum4(node, query, 0, dim4, dim16, 0);
			const float back = innerProdSum(node + dim4, query + dim4, dimLeft, 0, 0, 0);
			return 1.f - (front + back);
		}

		static float innerProduct16(
			const float* node, const float* query, const size_t, const size_t, const size_t dim16, const size_t
		) {
			return 1.f - innerProdSum16(node, query, 0, 0, dim16, 0);
		}

		static float innerProduct16Residual(
			const float* node, const float* query, const size_t, const size_t dim4, const size_t dim16, const size_t dimLeft
		) {
			const float front = innerProdSum16(node, query, 0, dim4, dim16, 0);
			const float back = innerProdSum(node + dim16, query + dim16, dimLeft, 0, 0, 0);
			return 1.f - (front + back);
		}

	#endif

	DistanceInfo getInnerProductInfo(const size_t dim, const size_t dim4, const size_t dim16) {
		size_t dimLeft = 0;
		DistanceFunction f = innerProduct;
		std::string name = "innerProduct";

		#if defined(USE_ENHANCED)
			if (dim % 16 == 0) {
				f = innerProduct16;
				name = "innerProduct16";
			}
			else if (dim % 4 == 0) {
				f = innerProduct4;
				name = "innerProduct4";
			}
			else if (dim > 16) {
				dimLeft = dim - dim16;
				f = innerProduct16Residual;
				name = "innerProduct16Residual";
			}
			else if (dim > 4) {
				dimLeft = dim - dim4;
				f = innerProduct4Residual;
				name = "innerProduct4Residual";
			}
		#endif

		return DistanceInfo(dimLeft, f, name);
	}
}
