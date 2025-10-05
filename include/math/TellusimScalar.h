// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#ifndef __TELLUSIM_MATH_SCALAR_H__
#define __TELLUSIM_MATH_SCALAR_H__

#include <cmath>

#include <TellusimBase.h>

/*
 */
namespace Tellusim {
	
	/**
	 * 32-bit Constants
	 */
	constexpr float32_t Pi = 3.141592653589793f;
	constexpr float32_t Pi2 = 6.283185307179586f;
	constexpr float32_t Pi05 = 1.570796326794897f;
	constexpr float32_t Sqrt2 = 1.4142135623730951f;
	constexpr float32_t Deg2Rad = 0.017453292519943f;
	constexpr float32_t Rad2Deg = 57.295779513082325f;
	
	/**
	 * 64-bit constants
	 */
	constexpr float64_t Pid = 3.141592653589793115997963468544;
	constexpr float64_t Pi2d = 6.283185307179586231995926937088;
	constexpr float64_t Pi05d = 1.570796326794896557998981734272;
	constexpr float64_t Sqrt2d = 1.414213562373095145474621858739;
	constexpr float64_t Deg2Radd = 0.017453292519943295474371680598;
	constexpr float64_t Rad2Degd = 57.295779513082322864647721871734;
	
	/**
	 * 8-bit signed integer numbers
	 */
	TS_INLINE int8_t min(int8_t v0, int8_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE int8_t max(int8_t v0, int8_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE int8_t clamp(int8_t v, int8_t v0, int8_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE int8_t select(int8_t v0, int8_t v1, int8_t s) { return (s & 0x80) ? v1 : v0; }
	
	/**
	 * 16-bit signed integer numbers
	 */
	TS_INLINE int16_t min(int16_t v0, int16_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE int16_t max(int16_t v0, int16_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE int16_t clamp(int16_t v, int16_t v0, int16_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE int16_t select(int16_t v0, int16_t v1, int16_t s) { return (s & 0x8000) ? v1 : v0; }
	
	/**
	 * 32-bit signed integer numbers
	 */
	TS_INLINE int32_t abs(int32_t x) { return (x > 0) ? x : -x; }
	TS_INLINE int32_t mod(int32_t x, int32_t y) { return x % y; }
	TS_INLINE int32_t sign(int32_t x) { if(x != 0) return (x > 0) ? 1 : -1; return 0; }
	
	TS_INLINE int32_t min(int32_t v0, int32_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE int32_t max(int32_t v0, int32_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE int32_t min(int32_t v0, int32_t v1, int32_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
	TS_INLINE int32_t max(int32_t v0, int32_t v1, int32_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
	TS_INLINE int32_t min(int32_t v0, int32_t v1, int32_t v2, int32_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
	TS_INLINE int32_t max(int32_t v0, int32_t v1, int32_t v2, int32_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
	TS_INLINE int32_t mid(int32_t v0, int32_t v1, int32_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
	TS_INLINE int32_t clamp(int32_t v, int32_t v0, int32_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE int32_t select(int32_t v0, int32_t v1, int32_t s) { return (s & (1 << 31)) ? v1 : v0; }
	
	/**
	 * 64-bit signed integer numbers
	 */
	TS_INLINE int64_t abs(int64_t x) { return (x > 0) ? x : -x; }
	TS_INLINE int64_t mod(int64_t x, int64_t y) { return x % y; }
	TS_INLINE int64_t sign(int64_t x) { if(x != 0) return (x > 0) ? 1 : -1; return 0; }
	
	TS_INLINE int64_t min(int64_t v0, int64_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE int64_t max(int64_t v0, int64_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE int64_t min(int64_t v0, int64_t v1, int64_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
	TS_INLINE int64_t max(int64_t v0, int64_t v1, int64_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
	TS_INLINE int64_t min(int64_t v0, int64_t v1, int64_t v2, int64_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
	TS_INLINE int64_t max(int64_t v0, int64_t v1, int64_t v2, int64_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
	TS_INLINE int64_t mid(int64_t v0, int64_t v1, int64_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
	TS_INLINE int64_t clamp(int64_t v, int64_t v0, int64_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE int64_t select(int64_t v0, int64_t v1, int64_t s) { return (s & (1ll << 63)) ? v1 : v0; }
	
	/**
	 * 8-bit unsigned integer numbers
	 */
	TS_INLINE uint8_t min(uint8_t v0, uint8_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE uint8_t max(uint8_t v0, uint8_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE uint8_t clamp(uint8_t v, uint8_t v0, uint8_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE uint8_t select(uint8_t v0, uint8_t v1, int8_t s) { return (s & 0x80) ? v1 : v0; }
	
	/**
	 * 16-bit unsigned integer numbers
	 */
	TS_INLINE uint16_t min(uint16_t v0, uint16_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE uint16_t max(uint16_t v0, uint16_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE uint16_t clamp(uint16_t v, uint16_t v0, uint16_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE uint16_t select(uint16_t v0, uint16_t v1, int16_t s) { return (s & 0x8000) ? v1 : v0; }
	
	/**
	 * 32-bit unsigned integer numbers
	 */
	TS_INLINE bool ispot(uint32_t x) { return ((x & (x - 1)) == 0); }
	TS_INLINE bool isaligned(uint32_t a, uint32_t b) { return ispot(b) ? ((a & (b - 1)) == 0) : ((a / b) * b == a); }
	TS_INLINE uint32_t npot(uint32_t x) { uint32_t ret = 1; while(ret < x) ret += ret; return ret; }
	TS_INLINE uint32_t log2(uint32_t x) { uint32_t ret = 0; while(x > 1) { x >>= 1; ret++; } return ret; }
	TS_INLINE uint32_t sqrt(uint32_t x) { uint32_t ret = 0, y = (1 << 30); while(y > x) y >>= 2; while(y) { if(x >= ret + y) { x -= ret + y; ret += y << 1; } ret >>= 1; y >>= 2; } return ret; }
	TS_INLINE uint32_t pow(uint32_t x, uint32_t y) { uint32_t ret = 1; while(y) { if(y & 1) ret *= x; x *= x; y >>= 1; } return ret; }
	TS_INLINE uint32_t align(uint32_t a, uint32_t b) { return ispot(b) ? ((a + b - 1) & ~(b - 1)) : (((a + b - 1) / b) * b); }
	TS_INLINE uint32_t udiv(uint32_t a, uint32_t b) { return (a + b - 1) / b; }
	
	TS_INLINE uint32_t min(uint32_t v0, uint32_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE uint32_t max(uint32_t v0, uint32_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE uint32_t min(uint32_t v0, uint32_t v1, uint32_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
	TS_INLINE uint32_t max(uint32_t v0, uint32_t v1, uint32_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
	TS_INLINE uint32_t min(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
	TS_INLINE uint32_t max(uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
	TS_INLINE uint32_t mid(uint32_t v0, uint32_t v1, uint32_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
	TS_INLINE uint32_t clamp(uint32_t v, uint32_t v0, uint32_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE uint32_t select(uint32_t v0, uint32_t v1, int32_t s) { return (s & (1 << 31)) ? v1 : v0; }
	
	/**
	 * 64-bit unsigned integer numbers
	 */
	TS_INLINE bool ispot(uint64_t x) { return ((x & (x - 1)) == 0); }
	TS_INLINE bool isaligned(uint64_t a, uint64_t b) { return ispot(b) ? ((a & (b - 1)) == 0) : ((a / b) * b == a); }
	TS_INLINE uint64_t npot(uint64_t x) { uint64_t ret = 1; while(ret < x) ret += ret; return ret; }
	TS_INLINE uint64_t log2(uint64_t x) { uint64_t ret = 0; while(x > 1) { x >>= 1; ret++; } return ret; }
	TS_INLINE uint64_t sqrt(uint64_t x) { uint64_t ret = 0, y = (1ull << 62); while(y > x) y >>= 2; while(y) { if(x >= ret + y) { x -= ret + y; ret += y << 1; } ret >>= 1; y >>= 2; } return ret; }
	TS_INLINE uint64_t pow(uint64_t x, uint32_t y) { uint64_t ret = 1; while(y) { if(y & 1) ret *= x; x *= x; y >>= 1; } return ret; }
	TS_INLINE uint64_t align(uint64_t a, uint64_t b) { return ispot(b) ? ((a + b - 1) & ~(b - 1)) : (((a + b - 1) / b) * b); }
	TS_INLINE uint64_t udiv(uint64_t a, uint64_t b) { return (a + b - 1) / b; }
	
	TS_INLINE uint64_t min(uint64_t v0, uint64_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE uint64_t max(uint64_t v0, uint64_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE uint64_t min(uint64_t v0, uint64_t v1, uint64_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
	TS_INLINE uint64_t max(uint64_t v0, uint64_t v1, uint64_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
	TS_INLINE uint64_t min(uint64_t v0, uint64_t v1, uint64_t v2, uint64_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
	TS_INLINE uint64_t max(uint64_t v0, uint64_t v1, uint64_t v2, uint64_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
	TS_INLINE uint64_t mid(uint64_t v0, uint64_t v1, uint64_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
	TS_INLINE uint64_t clamp(uint64_t v, uint64_t v0, uint64_t v1) { return min(max(v, v0), v1); }
	
	TS_INLINE uint64_t select(uint64_t v0, uint64_t v1, int64_t s) { return (s & (1ll << 63)) ? v1 : v0; }
	
	/**
	 * Platform-specific unsigned integer numbers
	 */
	#if _MACOS || _IOS || _TVOS || _EMSCRIPTEN
		
		TS_INLINE bool ispot(size_t x) { return ((x & (x - 1)) == 0); }
		TS_INLINE bool isaligned(size_t a, size_t b) { return ispot(b) ? ((a & (b - 1)) == 0) : ((a / b) * b == a); }
		TS_INLINE size_t npot(size_t x) { size_t ret = 1; while(ret < x) ret += ret; return ret; }
		TS_INLINE size_t log2(size_t x) { size_t ret = 0; while(x > 1) { x >>= 1; ret++; } return ret; }
		TS_INLINE size_t sqrt(size_t x) { size_t ret = 0, y = (1 << 30); while(y > x) y >>= 2; while(y) { if(x >= ret + y) { x -= ret + y; ret += y << 1; } ret >>= 1; y >>= 2; } return ret; }
		TS_INLINE size_t pow(size_t x, uint32_t y) { size_t ret = 1; while(y) { if(y & 1) ret *= x; x *= x; y >>= 1; } return ret; }
		TS_INLINE size_t align(size_t a, size_t b) { return ispot(b) ? ((a + b - 1) & ~(b - 1)) : (((a + b - 1) / b) * b); }
		TS_INLINE size_t udiv(size_t a, size_t b) { return (a + b - 1) / b; }
		
		TS_INLINE size_t min(size_t v0, size_t v1) { return (v0 < v1) ? v0 : v1; }
		TS_INLINE size_t max(size_t v0, size_t v1) { return (v0 < v1) ? v1 : v0; }
		TS_INLINE size_t min(size_t v0, size_t v1, size_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
		TS_INLINE size_t max(size_t v0, size_t v1, size_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
		TS_INLINE size_t min(size_t v0, size_t v1, size_t v2, size_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
		TS_INLINE size_t max(size_t v0, size_t v1, size_t v2, size_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
		TS_INLINE size_t mid(size_t v0, size_t v1, size_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
		TS_INLINE size_t clamp(size_t v, size_t v0, size_t v1) { return min(max(v, v0), v1); }
		
	#endif
	
	/**
	 * 32-bit floating-point numbers
	 */
	TS_INLINE float32_t sin(float32_t x) { return ::sinf(x); }
	TS_INLINE float32_t cos(float32_t x) { return ::cosf(x); }
	TS_INLINE float32_t tan(float32_t x) { return ::tanf(x); }
	TS_INLINE float32_t asin(float32_t x) { return ::asinf(x); }
	TS_INLINE float32_t acos(float32_t x) { return ::acosf(x); }
	TS_INLINE float32_t atan(float32_t x) { return ::atanf(x); }
	TS_INLINE float32_t atan2(float32_t y, float32_t x) { return ::atan2f(y, x); }
	TS_INLINE void sincos(float32_t x, float32_t &s, float32_t &c) { s = ::sinf(x); c = ::cosf(x); }
	
	TS_INLINE float32_t rcp(float32_t x) { return 1.0f / x; }
	TS_INLINE float32_t sqrt(float32_t x) { return ::sqrtf(x); }
	TS_INLINE float32_t rsqrt(float32_t x) { return 1.0f / ::sqrtf(x); }
	
	TS_INLINE float32_t pow(float32_t x, float32_t y) { return ::powf(x, y); }
	TS_INLINE float32_t exp(float32_t x) { return ::expf(x); }
	TS_INLINE float32_t log(float32_t x) { return ::logf(x); }
	TS_INLINE float32_t exp2(float32_t x) { return ::exp2f(x); }
	TS_INLINE float32_t log2(float32_t x) { return ::log2f(x); }
	TS_INLINE float32_t log10(float32_t x) { return ::log10f(x); }
	
	TS_INLINE float32_t abs(float32_t x) { return ::fabsf(x); }
	TS_INLINE float32_t ceil(float32_t x) { return ::ceilf(x); }
	TS_INLINE float32_t floor(float32_t x) { return ::floorf(x); }
	TS_INLINE float32_t round(float32_t x) { return ::roundf(x); }
	TS_INLINE float32_t fract(float32_t x) { return x - ::floorf(x); }
	TS_INLINE float32_t mod(float32_t x, float32_t y) { return ::fmodf(x, y); }
	TS_INLINE float32_t sign(float32_t x) { return (x > 0.0f) ? 1.0f : -1.0f; }
	
	TS_INLINE float32_t min(float32_t v0, float32_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE float32_t max(float32_t v0, float32_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE float32_t min(float32_t v0, float32_t v1, float32_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
	TS_INLINE float32_t max(float32_t v0, float32_t v1, float32_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
	TS_INLINE float32_t min(float32_t v0, float32_t v1, float32_t v2, float32_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
	TS_INLINE float32_t max(float32_t v0, float32_t v1, float32_t v2, float32_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
	TS_INLINE float32_t mid(float32_t v0, float32_t v1, float32_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
	TS_INLINE float32_t clamp(float32_t v, float32_t v0, float32_t v1) { return min(max(v, v0), v1); }
	TS_INLINE float32_t saturate(float32_t v) { return min(max(v, 0.0f), 1.0f); }
	
	TS_INLINE float32_t lerp(float32_t v0, float32_t v1, float32_t k) { return v0 + (v1 - v0) * k; }
	TS_INLINE float32_t smooth(float32_t v0, float32_t v1, float32_t v) { float32_t k = saturate((v - v0) / (v1 - v0)); return k * k * (3.0f - k * 2.0f); }
	
	TS_INLINE float32_t select(float32_t v0, float32_t v1, float32_t s) { return (s < 0.0f) ? v1 : v0; }
	
	TS_INLINE bool compare(float32_t v0, float32_t v1, float32_t a) { return (abs(v0 - v1) <= max(max(abs(v0), abs(v1)) * a, a)); }
	TS_INLINE bool compare(float32_t v0, float32_t v1, float32_t a, float32_t r) { return (abs(v0 - v1) <= max(max(abs(v0), abs(v1)) * r, a)); }
	
	/**
	 * 64-bit floating-point numbers
	 */
	TS_INLINE float64_t sin(float64_t x) { return ::sin(x); }
	TS_INLINE float64_t cos(float64_t x) { return ::cos(x); }
	TS_INLINE float64_t tan(float64_t x) { return ::tan(x); }
	TS_INLINE float64_t asin(float64_t x) { return ::asin(x); }
	TS_INLINE float64_t acos(float64_t x) { return ::acos(x); }
	TS_INLINE float64_t atan(float64_t x) { return ::atan(x); }
	TS_INLINE float64_t atan2(float64_t y, float64_t x) { return ::atan2(y, x); }
	TS_INLINE void sincos(float64_t x, float64_t &s, float64_t &c) { s = ::sin(x); c = ::cos(x); }
	
	TS_INLINE float64_t rcp(float64_t x) { return 1.0 / x; }
	TS_INLINE float64_t sqrt(float64_t x) { return ::sqrt(x); }
	TS_INLINE float64_t rsqrt(float64_t x) { return 1.0 / ::sqrt(x); }
	
	TS_INLINE float64_t pow(float64_t x, float64_t y) { return ::pow(x, y); }
	TS_INLINE float64_t exp(float64_t x) { return ::exp(x); }
	TS_INLINE float64_t log(float64_t x) { return ::log(x); }
	TS_INLINE float64_t exp2(float64_t x) { return ::exp2(x); }
	TS_INLINE float64_t log2(float64_t x) { return ::log2(x); }
	TS_INLINE float64_t log10(float64_t x) { return ::log10(x); }
	
	TS_INLINE float64_t abs(float64_t x) { return ::fabs(x); }
	TS_INLINE float64_t ceil(float64_t x) { return ::ceil(x); }
	TS_INLINE float64_t floor(float64_t x) { return ::floor(x); }
	TS_INLINE float64_t round(float64_t x) { return ::round(x); }
	TS_INLINE float64_t fract(float64_t x) { return x - ::floor(x); }
	TS_INLINE float64_t mod(float64_t x, float64_t y) { return ::fmod(x, y); }
	TS_INLINE float64_t sign(float64_t x) { return (x > 0.0) ? 1.0 : -1.0; }
	
	TS_INLINE float64_t min(float64_t v0, float64_t v1) { return (v0 < v1) ? v0 : v1; }
	TS_INLINE float64_t max(float64_t v0, float64_t v1) { return (v0 < v1) ? v1 : v0; }
	TS_INLINE float64_t min(float64_t v0, float64_t v1, float64_t v2) { return (v0 < v1) ? min(v0, v2) : min(v1, v2); }
	TS_INLINE float64_t max(float64_t v0, float64_t v1, float64_t v2) { return (v0 < v1) ? max(v1, v2) : max(v0, v2); }
	TS_INLINE float64_t min(float64_t v0, float64_t v1, float64_t v2, float64_t v3) { return (v0 < v1) ? min(v0, v2, v3) : min(v1, v2, v3); }
	TS_INLINE float64_t max(float64_t v0, float64_t v1, float64_t v2, float64_t v3) { return (v0 < v1) ? max(v1, v2, v3) : max(v0, v2, v3); }
	TS_INLINE float64_t mid(float64_t v0, float64_t v1, float64_t v2) { return (v0 < v1) ? ((v1 < v2) ? v1 : max(v0, v2)) : ((v0 < v2) ? v0 : max(v1, v2)); }
	TS_INLINE float64_t clamp(float64_t v, float64_t v0, float64_t v1) { return min(max(v, v0), v1); }
	TS_INLINE float64_t saturate(float64_t v) { return min(max(v, 0.0), 1.0); }
	
	TS_INLINE float64_t lerp(float64_t v0, float64_t v1, float64_t k) { return v0 + (v1 - v0) * k; }
	TS_INLINE float64_t smooth(float64_t v0, float64_t v1, float64_t v) { float64_t k = saturate((v - v0) / (v1 - v0)); return k * k * (3.0 - k * 2.0); }
	
	TS_INLINE float64_t select(float64_t v0, float64_t v1, float64_t s) { return (s < 0.0) ? v1 : v0; }
	
	TS_INLINE bool compare(float64_t v0, float64_t v1, float64_t a) { return (abs(v0 - v1) <= max(max(abs(v0), abs(v1)) * a, a)); }
	TS_INLINE bool compare(float64_t v0, float64_t v1, float64_t a, float64_t r) { return (abs(v0 - v1) <= max(max(abs(v0), abs(v1)) * r, a)); }
}

#endif /* __TELLUSIM_MATH_SCALAR_H__ */
