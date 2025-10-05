// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#include <math/TellusimMath.h>

#include "../include/TellusimCBase.h"

/*
 */
namespace Tellusim {
	
	/*
	 */
	#if TS_CALIGNED
		#define TS_DECLARE_TO_TYPE(TYPE, NAME) \
		static TS_INLINE const TYPE &to ## TYPE(const TS ## TYPE *v) { \
			return *(const TYPE*)v; \
		}
	#else
		#define TS_DECLARE_TO_TYPE(TYPE, NAME) \
		static TS_INLINE TYPE to ## TYPE(const TS ## TYPE *v) { \
			return TYPE(v->NAME); \
		}
	#endif
	
	TS_DECLARE_TO_TYPE(Vector4i, v)
	TS_DECLARE_TO_TYPE(Vector4u, v)
	TS_DECLARE_TO_TYPE(Vector4f, v)
	TS_DECLARE_TO_TYPE(Vector4d, v)
	
	TS_DECLARE_TO_TYPE(Matrix4x3f, m)
	TS_DECLARE_TO_TYPE(Matrix4x3d, m)
	TS_DECLARE_TO_TYPE(Matrix4x4f, m)
	TS_DECLARE_TO_TYPE(Matrix4x4d, m)
	
	TS_DECLARE_TO_TYPE(Quaternionf, q)
	TS_DECLARE_TO_TYPE(Quaterniond, q)
	
	/*
	 */
	extern "C" {
	
	/*****************************************************************************\
	 *
	 * Scalars
	 *
	\*****************************************************************************/
	
	/*
	 */
	TS_CAPI uint16_t TS_CCALL ts_f32tof16(float32_t f) {
		return float16_t(f).bits;
	}
	
	TS_CAPI float32_t TS_CCALL ts_f16tof32(uint16_t h) {
		return float16_t(h).get();
	}
	
	/*
	 */
	TS_CAPI uint32_t TS_CCALL ts_f32tof21(float32_t f) {
		return float21_t(f).bits;
	}
	
	TS_CAPI float32_t TS_CCALL ts_f21tof32(uint32_t h) {
		return float21_t(h).get();
	}
	
	/*
	 */
	TS_CAPI uint32_t TS_CCALL ts_f32tof24(float32_t f) {
		return float24_t(f).bits;
	}
	
	TS_CAPI float32_t TS_CCALL ts_f24tof32(uint32_t h) {
		return float24_t(h).get();
	}
	
	/*****************************************************************************\
	 *
	 * Vector
	 *
	\*****************************************************************************/
	
	/*
	 */
	#define TS_DECLARE_VECTOR_OP_V_VS(VECTOR, SCALAR, NAME, OP) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## VECTOR ## _ ## NAME(TS ## VECTOR *ret, const TS ## VECTOR *v, SCALAR s) { \
		(to ## VECTOR(v) OP s).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_VECTOR_IU_OP_V_VS(SIZE, NAME, OP) \
		TS_DECLARE_VECTOR_OP_V_VS(Vector ## SIZE ## i, int32_t, NAME, OP) \
		TS_DECLARE_VECTOR_OP_V_VS(Vector ## SIZE ## u, uint32_t, NAME, OP)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_OP_V_VV(VECTOR, NAME, OP) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## VECTOR ## _ ## NAME(TS ## VECTOR *ret, const TS ## VECTOR *v0, const TS ## VECTOR *v1) { \
		(to ## VECTOR(v0) OP to ## VECTOR(v1)).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_VECTOR_IU_OP_V_VV(SIZE, NAME, OP) \
		TS_DECLARE_VECTOR_OP_V_VS(Vector ## SIZE ## i, int32_t, NAME ## 1, OP) \
		TS_DECLARE_VECTOR_OP_V_VS(Vector ## SIZE ## u, uint32_t, NAME ## 1, OP) \
		TS_DECLARE_VECTOR_OP_V_VV(Vector ## SIZE ## i, NAME, OP) \
		TS_DECLARE_VECTOR_OP_V_VV(Vector ## SIZE ## u, NAME, OP)
	
	#define TS_DECLARE_VECTOR_FD_OP_V_VV(SIZE, NAME, OP) \
		TS_DECLARE_VECTOR_OP_V_VS(Vector ## SIZE ## f, float32_t, NAME ## 1, OP) \
		TS_DECLARE_VECTOR_OP_V_VS(Vector ## SIZE ## d, float64_t, NAME ## 1, OP) \
		TS_DECLARE_VECTOR_OP_V_VV(Vector ## SIZE ## f, NAME, OP) \
		TS_DECLARE_VECTOR_OP_V_VV(Vector ## SIZE ## d, NAME, OP)
	
	#define TS_DECLARE_VECTOR_IUFD_OP_V_VV(SIZE, NAME, OP) \
		TS_DECLARE_VECTOR_IU_OP_V_VV(SIZE, NAME, OP) \
		TS_DECLARE_VECTOR_FD_OP_V_VV(SIZE, NAME, OP)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_FUNC_S_V(VECTOR, SCALAR, NAME, FUNC) \
	TS_CAPI SCALAR TS_CCALL ts ## VECTOR ## _ ## NAME(const TS ## VECTOR *v) { \
		return FUNC(to ## VECTOR(v)); \
	}
	
	#define TS_DECLARE_VECTOR_FD_FUNC_S_V(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_S_V(Vector ## SIZE ## f, float32_t, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_S_V(Vector ## SIZE ## d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_FUNC_V_V(VECTOR, NAME, FUNC) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## VECTOR ## _ ## NAME(TS ## VECTOR *ret, const TS ## VECTOR *v) { \
		FUNC(to ## VECTOR(v)).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_VECTOR_IFD_FUNC_V_V(SIZE, NAME, FUNC) \
		TS_DECLARE_VECTOR_FUNC_V_V(Vector ## SIZE ## i, NAME, FUNC) \
		TS_DECLARE_VECTOR_FUNC_V_V(Vector ## SIZE ## f, NAME, FUNC) \
		TS_DECLARE_VECTOR_FUNC_V_V(Vector ## SIZE ## d, NAME, FUNC)
	
	#define TS_DECLARE_VECTOR_FD_FUNC_V_V(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_V(Vector ## SIZE ## f, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_V(Vector ## SIZE ## d, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_FUNC_S_VV(VECTOR, SCALAR, NAME, FUNC) \
	TS_CAPI SCALAR TS_CCALL ts ## VECTOR ## _ ## NAME(const TS ## VECTOR *v0, const TS ## VECTOR *v1) { \
		return FUNC(to ## VECTOR(v0), to ## VECTOR(v1)); \
	}
	
	#define TS_DECLARE_VECTOR_FD_FUNC_S_VV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_S_VV(Vector ## SIZE ## f, float32_t, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_S_VV(Vector ## SIZE ## d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_FUNC_V_VV(VECTOR, NAME, FUNC) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## VECTOR ## _ ## NAME(TS ## VECTOR *ret, const TS ## VECTOR *v0, const TS ## VECTOR *v1) { \
		FUNC(to ## VECTOR(v0), to ## VECTOR(v1)).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_VECTOR_IU_FUNC_V_VV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VV(Vector ## SIZE ## i, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VV(Vector ## SIZE ## u, NAME, NAME)
	
	#define TS_DECLARE_VECTOR_FD_FUNC_V_VV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VV(Vector ## SIZE ## f, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VV(Vector ## SIZE ## d, NAME, NAME)
	
	#define TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(SIZE, NAME) \
		TS_DECLARE_VECTOR_IU_FUNC_V_VV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FD_FUNC_V_VV(SIZE, NAME)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_FUNC_V_VVS(VECTOR, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## VECTOR ## _ ## NAME(TS ## VECTOR *ret, const TS ## VECTOR *v0, const TS ## VECTOR *v1, SCALAR s) { \
		FUNC(to ## VECTOR(v0), to ## VECTOR(v1), s).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_VECTOR_FD_FUNC_V_VVS(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VVS(Vector ## SIZE ## f, float32_t, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VVS(Vector ## SIZE ## d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_VECTOR_FUNC_V_VVV(VECTOR, NAME, FUNC) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## VECTOR ## _ ## NAME(TS ## VECTOR *ret, const TS ## VECTOR *v0, const TS ## VECTOR *v1, const TS ## VECTOR *v2) { \
		FUNC(to ## VECTOR(v0), to ## VECTOR(v1), to ## VECTOR(v2)).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_VECTOR_IU_FUNC_V_VVV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VVV(Vector ## SIZE ## i, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VVV(Vector ## SIZE ## u, NAME, NAME)
	
	#define TS_DECLARE_VECTOR_FD_FUNC_V_VVV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VVV(Vector ## SIZE ## f, NAME, NAME) \
		TS_DECLARE_VECTOR_FUNC_V_VVV(Vector ## SIZE ## d, NAME, NAME)
	
	#define TS_DECLARE_VECTOR_IUFD_FUNC_V_VVV(SIZE, NAME) \
		TS_DECLARE_VECTOR_IU_FUNC_V_VVV(SIZE, NAME) \
		TS_DECLARE_VECTOR_FD_FUNC_V_VVV(SIZE, NAME)
	
	/*****************************************************************************\
	 *
	 * Vector2
	 *
	\*****************************************************************************/
	
	/*
	 */
	#define TS_DECLARE_VECTOR_2(VECTOR, SCALAR) \
	TS_STATIC_ASSERT(sizeof(TS ## VECTOR) == sizeof(VECTOR)); \
	static const VECTOR &to ## VECTOR(const TS ## VECTOR *v) { \
		return *(const VECTOR*)v; \
	} \
	TS_CAPI TS ## VECTOR TS_CCALL ts ## VECTOR ## _new1(SCALAR v) { \
		TS ## VECTOR ret; ret.x = v; ret.y = v; \
		return ret; \
	} \
	TS_CAPI TS ## VECTOR TS_CCALL ts ## VECTOR ## _new(SCALAR x, SCALAR y) { \
		TS ## VECTOR ret; ret.x = x; ret.y = y; \
		return ret; \
	}
	
	/*
	 */
	TS_DECLARE_VECTOR_2(Vector2i, int32_t)
	TS_DECLARE_VECTOR_2(Vector2u, uint32_t)
	TS_DECLARE_VECTOR_2(Vector2f, float32_t)
	TS_DECLARE_VECTOR_2(Vector2d, float64_t)
	
	/*
	 */
	TS_DECLARE_VECTOR_IFD_FUNC_V_V(2, abs, abs)
	TS_DECLARE_VECTOR_IFD_FUNC_V_V(2, neg, operator-)
	
	/*
	 */
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(2, mul, *)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(2, div, /)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(2, add, +)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(2, sub, -)
	
	/*
	 */
	TS_DECLARE_VECTOR_IU_OP_V_VV(2, and, &)
	TS_DECLARE_VECTOR_IU_OP_V_VV(2,  or, |)
	TS_DECLARE_VECTOR_IU_OP_V_VV(2, xor, ^)
	TS_DECLARE_VECTOR_IU_OP_V_VS(2, shl1, <<)
	TS_DECLARE_VECTOR_IU_OP_V_VS(2, shr1, >>)
	
	/*
	 */
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(2, min)
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(2, max)
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VVV(2, clamp)
	
	/*
	 */
	TS_DECLARE_VECTOR_FD_FUNC_V_V(2, saturate)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(2, dot)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(2, cross)
	TS_DECLARE_VECTOR_FD_FUNC_S_V(2, length)
	TS_DECLARE_VECTOR_FD_FUNC_V_V(2, normalize)
	TS_DECLARE_VECTOR_FD_FUNC_V_VVS(2, lerp)
	
	/*****************************************************************************\
	 *
	 * Vector3
	 *
	\*****************************************************************************/
	
	/*
	 */
	#define TS_DECLARE_VECTOR_3(VECTOR, SCALAR) \
	TS_STATIC_ASSERT(sizeof(TS ## VECTOR) == sizeof(VECTOR)); \
	static const VECTOR &to ## VECTOR(const TS ## VECTOR *v) { \
		return *(const VECTOR*)v; \
	} \
	TS_CAPI TS ## VECTOR TS_CCALL ts ## VECTOR ## _new1(SCALAR v) { \
		TS ## VECTOR ret; ret.x = v; ret.y = v; ret.z = v; \
		return ret; \
	} \
	TS_CAPI TS ## VECTOR TS_CCALL ts ## VECTOR ## _new(SCALAR x, SCALAR y, SCALAR z) { \
		TS ## VECTOR ret; ret.x = x; ret.y = y; ret.z = z; \
		return ret; \
	}
	
	/*
	 */
	TS_DECLARE_VECTOR_3(Vector3i, int32_t)
	TS_DECLARE_VECTOR_3(Vector3u, uint32_t)
	TS_DECLARE_VECTOR_3(Vector3f, float32_t)
	TS_DECLARE_VECTOR_3(Vector3d, float64_t)
	
	/*
	 */
	TS_DECLARE_VECTOR_IFD_FUNC_V_V(3, abs, abs)
	TS_DECLARE_VECTOR_IFD_FUNC_V_V(3, neg, operator-)
	
	/*
	 */
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(3, mul, *)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(3, div, /)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(3, add, +)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(3, sub, -)
	
	/*
	 */
	TS_DECLARE_VECTOR_IU_OP_V_VV(3, and, &)
	TS_DECLARE_VECTOR_IU_OP_V_VV(3,  or, |)
	TS_DECLARE_VECTOR_IU_OP_V_VV(3, xor, ^)
	TS_DECLARE_VECTOR_IU_OP_V_VS(3, shl1, <<)
	TS_DECLARE_VECTOR_IU_OP_V_VS(3, shr1, >>)
	
	/*
	 */
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(3, min)
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(3, max)
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VVV(3, clamp)
	
	/*
	 */
	TS_DECLARE_VECTOR_FD_FUNC_V_V(3, saturate)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(3, dot)
	TS_DECLARE_VECTOR_FD_FUNC_V_VV(3, cross)
	TS_DECLARE_VECTOR_FD_FUNC_S_V(3, length)
	TS_DECLARE_VECTOR_FD_FUNC_V_V(3, normalize)
	TS_DECLARE_VECTOR_FD_FUNC_V_VVS(3, lerp)
	
	/*****************************************************************************\
	 *
	 * Vector4
	 *
	\*****************************************************************************/
	
	/*
	 */
	#define TS_DECLARE_VECTOR_4(VECTOR, SCALAR) \
	TS_STATIC_ASSERT(sizeof(TS ## VECTOR) == sizeof(VECTOR)); \
	TS_CAPI TS ## VECTOR TS_CCALL ts ## VECTOR ## _new1(SCALAR v) { \
		TS ## VECTOR ret; ret.x = v; ret.y = v; ret.z = v; ret.w = v; \
		return ret; \
	} \
	TS_CAPI TS ## VECTOR TS_CCALL ts ## VECTOR ## _new(SCALAR x, SCALAR y, SCALAR z, SCALAR w) { \
		TS ## VECTOR ret; ret.x = x; ret.y = y; ret.z = z; ret.w = w; \
		return ret; \
	}
	
	/*
	 */
	TS_DECLARE_VECTOR_4(Vector4i, int32_t)
	TS_DECLARE_VECTOR_4(Vector4u, uint32_t)
	TS_DECLARE_VECTOR_4(Vector4f, float32_t)
	TS_DECLARE_VECTOR_4(Vector4d, float64_t)
	
	/*
	 */
	TS_DECLARE_VECTOR_IFD_FUNC_V_V(4, abs, abs)
	TS_DECLARE_VECTOR_IFD_FUNC_V_V(4, neg, operator-)
	
	/*
	 */
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(4, mul, *)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(4, div, /)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(4, add, +)
	TS_DECLARE_VECTOR_IUFD_OP_V_VV(4, sub, -)
	
	/*
	 */
	TS_DECLARE_VECTOR_IU_OP_V_VV(4, and, &)
	TS_DECLARE_VECTOR_IU_OP_V_VV(4,  or, |)
	TS_DECLARE_VECTOR_IU_OP_V_VV(4, xor, ^)
	TS_DECLARE_VECTOR_IU_OP_V_VS(4, shl1, <<)
	TS_DECLARE_VECTOR_IU_OP_V_VS(4, shr1, >>)
	
	/*
	 */
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(4, min)
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VV(4, max)
	TS_DECLARE_VECTOR_IUFD_FUNC_V_VVV(4, clamp)
	
	/*
	 */
	TS_DECLARE_VECTOR_FD_FUNC_V_V(4, saturate)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(4, dot)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(4, dot33)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(4, dot43)
	TS_DECLARE_VECTOR_FD_FUNC_S_VV(4, dot34)
	TS_DECLARE_VECTOR_FD_FUNC_V_VV(4, cross)
	TS_DECLARE_VECTOR_FD_FUNC_S_V(4, length)
	TS_DECLARE_VECTOR_FD_FUNC_S_V(4, length3)
	TS_DECLARE_VECTOR_FD_FUNC_V_V(4, normalize)
	TS_DECLARE_VECTOR_FD_FUNC_V_V(4, normalize3)
	TS_DECLARE_VECTOR_FD_FUNC_V_VVS(4, lerp)
	
	/*****************************************************************************\
	 *
	 * Matrix
	 *
	\*****************************************************************************/
	
	/*
	 */
	#define TS_DECLARE_MATRIX_OP_V_M(MATRIX, VECTOR, NUM, NAME, OP) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## MATRIX ## _ ## NAME ## NUM(TS ## VECTOR *ret, const TS ## MATRIX *m, const TS ## VECTOR *v) { \
		(to ## MATRIX(m) OP to ## VECTOR(v)).get(ret->v); \
		return ret; \
	} \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## MATRIX ## _ ## NAME ## NUM ## t(TS ## VECTOR *ret, const TS ## VECTOR *v, const TS ## MATRIX *m) { \
		(to ## VECTOR(v) OP to ## MATRIX(m)).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_OP_M_MM(MATRIX, NAME, OP) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, const TS ## MATRIX *m0, const TS ## MATRIX *m1) { \
		(to ## MATRIX(m0) * to ## MATRIX(m1)).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_OP(SIZE, NAME, OP) \
		TS_DECLARE_MATRIX_OP_V_M(Matrix ## SIZE ## f, Vector2f, 2, NAME, OP) \
		TS_DECLARE_MATRIX_OP_V_M(Matrix ## SIZE ## d, Vector2d, 2, NAME, OP) \
		TS_DECLARE_MATRIX_OP_V_M(Matrix ## SIZE ## f, Vector3f, 3, NAME, OP) \
		TS_DECLARE_MATRIX_OP_V_M(Matrix ## SIZE ## d, Vector3d, 3, NAME, OP) \
		TS_DECLARE_MATRIX_OP_V_M(Matrix ## SIZE ## f, Vector4f, 4, NAME, OP) \
		TS_DECLARE_MATRIX_OP_V_M(Matrix ## SIZE ## d, Vector4d, 4, NAME, OP) \
		TS_DECLARE_MATRIX_OP_M_MM(Matrix ## SIZE ## f, NAME, OP) \
		TS_DECLARE_MATRIX_OP_M_MM(Matrix ## SIZE ## d, NAME, OP)
	
	/*
	 */
	#define TS_DECLARE_MATRIX_FUNC_M_S(MATRIX, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, SCALAR s) { \
		MATRIX::FUNC(s).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_FUNC_M_S(SIZE, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_S(Matrix ## SIZE ## f, float32_t, NAME, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_S(Matrix ## SIZE ## d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_MATRIX_FUNC_M_M(MATRIX, NAME, FUNC) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, const TS ## MATRIX *m) { \
		FUNC(to ## MATRIX(m)).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_FUNC_M_M(SIZE, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_M(Matrix ## SIZE ## f, NAME, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_M(Matrix ## SIZE ## d, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_MATRIX_FUNC_M_VS(MATRIX, VECTOR, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, const TS ## VECTOR *v, SCALAR s) { \
		MATRIX::FUNC(to ## VECTOR(v), s).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_FUNC_M_VS(SIZE, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_VS(Matrix ## SIZE ## f, Vector3f, float32_t, NAME, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_VS(Matrix ## SIZE ## d, Vector3d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_MATRIX_FUNC_M_SSS(MATRIX, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, SCALAR s0, SCALAR s1, SCALAR s2) { \
		MATRIX::FUNC(s0, s1, s2).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_FUNC_M_SSS(SIZE, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_SSS(Matrix ## SIZE ## f, float32_t, NAME, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_SSS(Matrix ## SIZE ## d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_MATRIX_FUNC_M_VVV(MATRIX, VECTOR, NAME, FUNC) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, const TS ## VECTOR *v0, const TS ## VECTOR *v1, const TS ## VECTOR *v2) { \
		MATRIX::FUNC(to ## VECTOR(v0), to ## VECTOR(v1), to ## VECTOR(v2)).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_FUNC_M_VVV(SIZE, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_VVV(Matrix ## SIZE ## f, Vector3f, NAME, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_VVV(Matrix ## SIZE ## d, Vector3d, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_MATRIX_FUNC_M_MMS(MATRIX, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## MATRIX* TS_CCALL ts ## MATRIX ## _ ## NAME(TS ## MATRIX *ret, const TS ## MATRIX *m0, const TS ## MATRIX *m1, SCALAR s) { \
		FUNC(to ## MATRIX(m0), to ## MATRIX(m1), s).get(ret->m); \
		return ret; \
	}
	
	#define TS_DECLARE_MATRIX_FD_FUNC_M_MMS(SIZE, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_MMS(Matrix ## SIZE ## f, float32_t, NAME, NAME) \
		TS_DECLARE_MATRIX_FUNC_M_MMS(Matrix ## SIZE ## d, float64_t, NAME, NAME)
	
	/*****************************************************************************\
	 *
	 * Matrix3x2
	 *
	\*****************************************************************************/
	
	/*
	 */
	TS_STATIC_ASSERT(sizeof(TSMatrix3x2f) == sizeof(Matrix3x2f));
	TS_STATIC_ASSERT(sizeof(TSMatrix3x2d) == sizeof(Matrix3x2d));
	
	/*
	 */
	static const Matrix3x2f &toMatrix3x2f(const TSMatrix3x2f *m) {
		return *(const Matrix3x2f*)m;
	}
	
	static const Matrix3x2d &toMatrix3x2d(const TSMatrix3x2d *m) {
		return *(const Matrix3x2d*)m;
	}
	
	/*
	 */
	TS_CAPI void TS_CCALL tsMatrix3x2f_set(TSMatrix3x2f *m, const float32_t *src, uint32_t size, bool_t row_major) {
		((Matrix3x2f*)m)->set(src, size, (bool)row_major);
	}
	
	TS_CAPI void TS_CCALL tsMatrix3x2d_set(TSMatrix3x2d *m, const float64_t *src, uint32_t size, bool_t row_major) {
		((Matrix3x2d*)m)->set(src, size, (bool)row_major);
	}
	
	/*
	 */
	TS_CAPI void TS_CCALL tsMatrix3x2f_get(const TSMatrix3x2f *m, float32_t *dest, uint32_t size, bool_t row_major) {
		toMatrix3x2f(m).get(dest, size, (bool)row_major);
	}
	
	TS_CAPI void TS_CCALL tsMatrix3x2d_get(const TSMatrix3x2d *m, float64_t *dest, uint32_t size, bool_t row_major) {
		toMatrix3x2d(m).get(dest, size, (bool)row_major);
	}
	
	/*
	 */
	TS_CAPI TSMatrix3x2f* TS_CCALL tsMatrix3x2f_scale(TSMatrix3x2f *ret, float32_t x, float32_t y) {
		Matrix3x2f::scale(x, y).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix3x2d* TS_CCALL tsMatrix3x2d_scale(TSMatrix3x2d *ret, float64_t x, float64_t y) {
		Matrix3x2d::scale(x, y).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_CAPI TSMatrix3x2f* TS_CCALL tsMatrix3x2f_translate(TSMatrix3x2f *ret, float32_t x, float32_t y) {
		Matrix3x2f::translate(x, y).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix3x2d* TS_CCALL tsMatrix3x2d_translate(TSMatrix3x2d *ret, float64_t x, float64_t y) {
		Matrix3x2d::translate(x, y).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_S(3x2, rotate)
	
	/*
	 */
	TS_DECLARE_MATRIX_OP_V_M(Matrix3x2f, Vector2f, 2, mul, *)
	TS_DECLARE_MATRIX_OP_V_M(Matrix3x2d, Vector2d, 2, mul, *)
	TS_DECLARE_MATRIX_OP_M_MM(Matrix3x2f, mul, *)
	TS_DECLARE_MATRIX_OP_M_MM(Matrix3x2d, mul, *)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_M(3x2, transpose)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(3x2, inverse)
	
	/*****************************************************************************\
	 *
	 * Matrix4x3
	 *
	\*****************************************************************************/
	
	/*
	 */
	TS_STATIC_ASSERT(sizeof(TSMatrix4x3f) == sizeof(Matrix4x3f));
	TS_STATIC_ASSERT(sizeof(TSMatrix4x3d) == sizeof(Matrix4x3d));
	
	/*
	 */
	TS_CAPI void TS_CCALL tsMatrix4x3f_set(TSMatrix4x3f *m, const float32_t *src, uint32_t size, bool_t row_major) {
		((Matrix4x3f*)m)->set(src, size, (bool)row_major);
	}
	
	TS_CAPI void TS_CCALL tsMatrix4x3d_set(TSMatrix4x3d *m, const float64_t *src, uint32_t size, bool_t row_major) {
		((Matrix4x3d*)m)->set(src, size, (bool)row_major);
	}
	
	/*
	 */
	TS_CAPI void TS_CCALL tsMatrix4x3f_get(const TSMatrix4x3f *m, float32_t *dest, uint32_t size, bool_t row_major) {
		toMatrix4x3f(m).get(dest, size, (bool)row_major);
	}
	
	TS_CAPI void TS_CCALL tsMatrix4x3d_get(const TSMatrix4x3d *m, float64_t *dest, uint32_t size, bool_t row_major) {
		toMatrix4x3d(m).get(dest, size, (bool)row_major);
	}
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_SSS(4x3, scale)
	TS_DECLARE_MATRIX_FD_FUNC_M_SSS(4x3, translate)
	TS_DECLARE_MATRIX_FD_FUNC_M_S(4x3, rotateX)
	TS_DECLARE_MATRIX_FD_FUNC_M_S(4x3, rotateY)
	TS_DECLARE_MATRIX_FD_FUNC_M_S(4x3, rotateZ)
	TS_DECLARE_MATRIX_FD_FUNC_M_VS(4x3, rotate)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_VVV(4x3, lookAt)
	TS_DECLARE_MATRIX_FD_FUNC_M_VVV(4x3, placeTo)
	
	/*
	 */
	TS_CAPI TSMatrix4x3f* TS_CCALL tsMatrix4x3f_compose(TSMatrix4x3f *ret, const TSVector3f *t, const TSQuaternionf *r, const TSVector3f *s) {
		Matrix4x3f::compose(toVector3f(t), toQuaternionf(r), toVector3f(s)).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x3d* TS_CCALL tsMatrix4x3d_compose(TSMatrix4x3d *ret, const TSVector3d *t, const TSQuaterniond *r, const TSVector3d *s) {
		Matrix4x3d::compose(toVector3d(t), toQuaterniond(r), toVector3d(s)).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_CAPI TSMatrix4x3f* TS_CCALL tsMatrix4x3f_basis(TSMatrix4x3f *ret, const TSVector3f *normal, const TSVector3f *t) {
		Matrix4x3f::basis(toVector3f(normal), toVector3f(t)).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x3d* TS_CCALL tsMatrix4x3d_basis(TSMatrix4x3d *ret, const TSVector3d *normal, const TSVector3d *t) {
		Matrix4x3d::basis(toVector3d(normal), toVector3d(t)).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_OP(4x3, mul, *)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x3, normalize)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x3, transpose)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x3, inverse)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x3, inverse33)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_MMS(4x3, lerp)
	
	/*****************************************************************************\
	 *
	 * Matrix4x4
	 *
	\*****************************************************************************/
	
	/*
	 */
	TS_STATIC_ASSERT(sizeof(TSMatrix4x4f) == sizeof(Matrix4x4f));
	TS_STATIC_ASSERT(sizeof(TSMatrix4x4d) == sizeof(Matrix4x4d));
	
	/*
	 */
	TS_CAPI void TS_CCALL tsMatrix4x4f_set(TSMatrix4x4f *m, const float32_t *src, uint32_t size, bool_t row_major) {
		((Matrix4x4f*)m)->set(src, size, (bool)row_major);
	}
	
	TS_CAPI void TS_CCALL tsMatrix4x4d_set(TSMatrix4x4d *m, const float64_t *src, uint32_t size, bool_t row_major) {
		((Matrix4x4d*)m)->set(src, size, (bool)row_major);
	}
	
	/*
	 */
	TS_CAPI void TS_CCALL tsMatrix4x4f_get(const TSMatrix4x4f *m, float32_t *dest, uint32_t size, bool_t row_major) {
		toMatrix4x4f(m).get(dest, size, (bool)row_major);
	}
	
	TS_CAPI void TS_CCALL tsMatrix4x4d_get(const TSMatrix4x4d *m, float64_t *dest, uint32_t size, bool_t row_major) {
		toMatrix4x4d(m).get(dest, size, (bool)row_major);
	}
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_SSS(4x4, scale)
	TS_DECLARE_MATRIX_FD_FUNC_M_SSS(4x4, translate)
	TS_DECLARE_MATRIX_FD_FUNC_M_S(4x4, rotateX)
	TS_DECLARE_MATRIX_FD_FUNC_M_S(4x4, rotateY)
	TS_DECLARE_MATRIX_FD_FUNC_M_S(4x4, rotateZ)
	TS_DECLARE_MATRIX_FD_FUNC_M_VS(4x4, rotate)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_VVV(4x4, lookAt)
	TS_DECLARE_MATRIX_FD_FUNC_M_VVV(4x4, placeTo)
	
	/*
	 */
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_compose(TSMatrix4x4f *ret, const TSVector3f *t, const TSQuaternionf *r, const TSVector3f *s) {
		Matrix4x4f::compose(toVector3f(t), toQuaternionf(r), toVector3f(s)).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_compose(TSMatrix4x4d *ret, const TSVector3d *t, const TSQuaterniond *r, const TSVector3d *s) {
		Matrix4x4d::compose(toVector3d(t), toQuaterniond(r), toVector3d(s)).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_basis(TSMatrix4x4f *ret, const TSVector3f *normal, const TSVector3f *t) {
		Matrix4x4f::basis(toVector3f(normal), toVector3f(t)).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_basis(TSMatrix4x4d *ret, const TSVector3d *normal, const TSVector3d *t) {
		Matrix4x4d::basis(toVector3d(normal), toVector3d(t)).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_ortho(TSMatrix4x4f *ret, float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t znear, float32_t zfar) {
		Matrix4x4f::ortho(left, right, bottom, top, znear, zfar).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_ortho(TSMatrix4x4d *ret, float64_t left, float64_t right, float64_t bottom, float64_t top, float64_t znear, float64_t zfar) {
		Matrix4x4d::ortho(left, right, bottom, top, znear, zfar).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_frustum(TSMatrix4x4f *ret, float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t znear, float32_t zfar, bool_t reverse) {
		Matrix4x4f::frustum(left, right, bottom, top, znear, zfar, (bool)reverse).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_frustum(TSMatrix4x4d *ret, float64_t left, float64_t right, float64_t bottom, float64_t top, float64_t znear, float64_t zfar, bool_t reverse) {
		Matrix4x4d::frustum(left, right, bottom, top, znear, zfar, (bool)reverse).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_frustumi(TSMatrix4x4f *ret, float32_t left, float32_t right, float32_t bottom, float32_t top, float32_t znear, bool_t reverse) {
		Matrix4x4f::frustum(left, right, bottom, top, znear, (bool)reverse).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_frustumi(TSMatrix4x4d *ret, float64_t left, float64_t right, float64_t bottom, float64_t top, float64_t znear, bool_t reverse) {
		Matrix4x4d::frustum(left, right, bottom, top, znear, (bool)reverse).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_perspective(TSMatrix4x4f *ret, float32_t fov, float32_t aspect, float32_t znear, float32_t zfar, bool_t reverse) {
		Matrix4x4f::perspective(fov, aspect, znear, zfar, (bool)reverse).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_perspective(TSMatrix4x4d *ret, float64_t fov, float64_t aspect, float64_t znear, float64_t zfar, bool_t reverse) {
		Matrix4x4d::perspective(fov, aspect, znear, zfar, (bool)reverse).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4f* TS_CCALL tsMatrix4x4f_perspectivei(TSMatrix4x4f *ret, float32_t fov, float32_t aspect, float32_t znear, bool_t reverse) {
		Matrix4x4f::perspective(fov, aspect, znear, (bool)reverse).get(ret->m);
		return ret;
	}
	
	TS_CAPI TSMatrix4x4d* TS_CCALL tsMatrix4x4d_perspectivei(TSMatrix4x4d *ret, float64_t fov, float64_t aspect, float64_t znear, bool_t reverse) {
		Matrix4x4d::perspective(fov, aspect, znear, (bool)reverse).get(ret->m);
		return ret;
	}
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_OP(4x4, mul, *)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x4, normalize)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x4, transpose)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x4, inverse)
	TS_DECLARE_MATRIX_FD_FUNC_M_M(4x4, inverse43)
	
	/*
	 */
	TS_DECLARE_MATRIX_FD_FUNC_M_MMS(4x4, lerp)
	
	/*****************************************************************************\
	 *
	 * Quaternion
	 *
	\*****************************************************************************/
	
	/*
	 */
	#define TS_DECLARE_QUATERNION_OP_V_Q(QUATERNION, VECTOR, NUM, NAME, OP) \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## QUATERNION ## _ ## NAME ## NUM(TS ## VECTOR *ret, const TS ## QUATERNION *q, const TS ## VECTOR *v) { \
		(to ## QUATERNION(q) OP to ## VECTOR(v)).get(ret->v); \
		return ret; \
	} \
	TS_CAPI TS ## VECTOR* TS_CCALL ts ## QUATERNION ## _ ## NAME ## NUM ## t(TS ## VECTOR *ret, const TS ## VECTOR *v, const TS ## QUATERNION *q) { \
		(to ## VECTOR(v) OP to ## QUATERNION(q)).get(ret->v); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_OP_Q_QS(QUATERNION, SCALAR, NAME, OP) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME ## 1(TS ## QUATERNION *ret, const TS ## QUATERNION *q, SCALAR s) { \
		(to ## QUATERNION(q) * s).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_OP_Q_QQ(QUATERNION, NAME, OP) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME(TS ## QUATERNION *ret, const TS ## QUATERNION *q0, const TS ## QUATERNION *q1) { \
		(to ## QUATERNION(q0) * to ## QUATERNION(q1)).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_FD_OP(NAME, OP) \
		TS_DECLARE_QUATERNION_OP_V_Q(Quaternionf, Vector2f, 2, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_V_Q(Quaterniond, Vector2d, 2, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_V_Q(Quaternionf, Vector3f, 3, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_V_Q(Quaterniond, Vector3d, 3, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_V_Q(Quaternionf, Vector4f, 4, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_V_Q(Quaterniond, Vector4d, 4, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_Q_QS(Quaternionf, float32_t, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_Q_QS(Quaterniond, float64_t, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_Q_QQ(Quaternionf, NAME, OP) \
		TS_DECLARE_QUATERNION_OP_Q_QQ(Quaterniond, NAME, OP)
	
	/*
	 */
	#define TS_DECLARE_QUATERNION_FUNC_Q_S(QUATERNION, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME(TS ## QUATERNION *ret, SCALAR s) { \
		QUATERNION::FUNC(s).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_FD_FUNC_Q_S(NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_S(Quaternionf, float32_t, NAME, NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_S(Quaterniond, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_QUATERNION_FUNC_Q_V(QUATERNION, VECTOR, NAME, FUNC) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME(TS ## QUATERNION *ret, const TS ## VECTOR *v) { \
		QUATERNION::FUNC(to ## VECTOR(v)).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_FD_FUNC_Q_V(NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_V(Quaternionf, Vector3f, NAME, NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_V(Quaterniond, Vector3d, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_QUATERNION_FUNC_Q_Q(QUATERNION, NAME, FUNC) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME(TS ## QUATERNION *ret, const TS ## QUATERNION *q) { \
		FUNC(to ## QUATERNION(q)).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_FD_FUNC_Q_Q(NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_Q(Quaternionf, NAME, NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_Q(Quaterniond, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_QUATERNION_FUNC_Q_VS(QUATERNION, VECTOR, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME(TS ## QUATERNION *ret, const TS ## VECTOR *v, SCALAR s) { \
		QUATERNION::FUNC(to ## VECTOR(v), s).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_FD_FUNC_Q_VS(NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_VS(Quaternionf, Vector3f, float32_t, NAME, NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_VS(Quaterniond, Vector3d, float64_t, NAME, NAME)
	
	/*
	 */
	#define TS_DECLARE_QUATERNION_FUNC_Q_QQS(QUATERNION, SCALAR, NAME, FUNC) \
	TS_CAPI TS ## QUATERNION* TS_CCALL ts ## QUATERNION ## _ ## NAME(TS ## QUATERNION *ret, const TS ## QUATERNION *q0, const TS ## QUATERNION *q1, SCALAR s) { \
		FUNC(to ## QUATERNION(q0), to ## QUATERNION(q1), s).get(ret->q); \
		return ret; \
	}
	
	#define TS_DECLARE_QUATERNION_FD_FUNC_Q_QQS(NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_QQS(Quaternionf, float32_t, NAME, NAME) \
		TS_DECLARE_QUATERNION_FUNC_Q_QQS(Quaterniond, float64_t, NAME, NAME)
	
	/*****************************************************************************\
	 *
	 * Quaternion
	 *
	\*****************************************************************************/
	
	/*
	 */
	TS_STATIC_ASSERT(sizeof(TSQuaternionf) == sizeof(Quaternionf));
	TS_STATIC_ASSERT(sizeof(TSQuaterniond) == sizeof(Quaterniond));
	
	/*
	 */
	TS_CAPI TSQuaternionf TS_CCALL tsQuaternionf_new(float32_t x, float32_t y, float32_t z, float32_t w) {
		TSQuaternionf ret; ret.x = x; ret.y = y; ret.z = z; ret.w = w;
		return ret;
	}
	
	TS_CAPI TSQuaterniond TS_CCALL tsQuaterniond_new(float64_t x, float64_t y, float64_t z, float64_t w) {
		TSQuaterniond ret; ret.x = x; ret.y = y; ret.z = z; ret.w = w;
		return ret;
	}
	
	/*
	 */
	TS_CAPI void TS_CCALL tsQuaternionf_set(TSQuaternionf *q, const TSVector4f *row_0, const TSVector4f *row_1, const TSVector4f *row_2) {
		((Quaternionf*)q)->set(toVector4f(row_0), toVector4f(row_1), toVector4f(row_2));
	}
	
	TS_CAPI void TS_CCALL tsQuaterniond_set(TSQuaterniond *q, const TSVector4d *row_0, const TSVector4d *row_1, const TSVector4d *row_2) {
		((Quaterniond*)q)->set(toVector4d(row_0), toVector4d(row_1), toVector4d(row_2));
	}
	
	/*
	 */
	TS_CAPI void TS_CCALL tsQuaternionf_get(const TSQuaternionf *q, TSVector4f *row_0, TSVector4f *row_1, TSVector4f *row_2) {
		toQuaternionf(q).get(*(Vector4f*)row_0, *(Vector4f*)row_1, *(Vector4f*)row_2);
	}
	
	TS_CAPI void TS_CCALL tsQuaterniond_get(const TSQuaterniond *q, TSVector4d *row_0, TSVector4d *row_1, TSVector4d *row_2) {
		toQuaterniond(q).get(*(Vector4d*)row_0, *(Vector4d*)row_1, *(Vector4d*)row_2);
	}
	
	/*
	 */
	TS_DECLARE_QUATERNION_FD_FUNC_Q_S(rotateX)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_S(rotateY)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_S(rotateZ)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_V(rotateXYZ)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_V(rotateZYX)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_VS(rotate)
	
	/*
	 */
	TS_DECLARE_QUATERNION_FD_OP(mul, *)
	
	/*
	 */
	TS_DECLARE_QUATERNION_FD_FUNC_Q_Q(normalize)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_Q(inverse)
	
	/*
	 */
	TS_DECLARE_QUATERNION_FD_FUNC_Q_QQS(lerp)
	TS_DECLARE_QUATERNION_FD_FUNC_Q_QQS(slerp)
	
	} /* extern "C" */
}
