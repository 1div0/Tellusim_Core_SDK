// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#version 430 core

/*
 */
#if COMPUTE_SHADER
	
	layout(local_size_x = 64) in;
	
	layout(std430, binding = 0) buffer ReadBuffer { uvec2 read_buffer[]; };
	layout(std430, binding = 1) buffer WriteBuffer { uvec2 write_buffer[]; };
	
	/*
	 */
	uvec2 u64_mul(uint a, uint b) {
		#if CLAY_VK || CLAY_D3D12 || CLAY_MTL || CLAY_CU || CLAY_HIP
			uint64_t v = uint64_t(a) * uint64_t(b);
			return uvec2(uint(v >> 32u), uint(v));
		#else
			return uvec2(0u);
		#endif
	}
	
	ivec2 i64_mul(int a, int b) {
		#if CLAY_VK || CLAY_D3D12 || CLAY_MTL || CLAY_CU || CLAY_HIP
			int64_t v = int64_t(a) * int64_t(b);
			return ivec2(int(v >> 32u), int(v));
		#else
			return uvec2(0u);
		#endif
	}
	
	uvec2 u32_mul(uint a, uint b) {
		uvec4 v = uvec2(a >> 16u, a & 0xffffu).xxyy * uvec2(b >> 16u, b & 0xffffu).xyxy;
		uint high = v.x + (v.y >> 16u) + (v.z >> 16u) + (((v.w >> 16u) + (v.y & 0xffffu) + (v.z & 0xffffu)) >> 16u);
		return uvec2(high, a * b);
	}
	
	ivec2 i32_mul(int a, int b) {
		uvec4 v = uvec2(uint(a) >> 16u, uint(a) & 0xffffu).xxyy * uvec2(uint(b) >> 16u, uint(b) & 0xffffu).xyxy;
		int high = int(v.x + (v.y >> 16u) + (v.z >> 16u) + (((v.w >> 16u) + (v.y & 0xffffu) + (v.z & 0xffffu)) >> 16u));
		high -= ((a < 0) ? b : 0) + ((b < 0) ? a : 0);
		return ivec2(high, a * b);
	}
	
	uvec2 umulExt(uint a, uint b) {
		uint high, low;
		umulExtended(a, b, high, low);
		return uvec2(high, low);
	}
	
	ivec2 imulExt(int a, int b) {
		int high, low;
		imulExtended(a, b, high, low);
		return ivec2(high, low);
	}
	
	uvec2 uaddExt(uint a, uint b) {
		uint high;
		uint low = uaddCarry(a, b, high);
		return uvec2(high, low);
	}
	
	uvec2 usubExt(uint a, uint b) {
		uint high;
		uint low = usubBorrow(a, b, high);
		return uvec2(high, low);
	}
	
	/*
	 */
	void main() {
		
		int global_id = gl_GlobalInvocationID.x;
		int index = global_id;
		int dest = 0;
		int src = 0;
		
		// u32 * u32
		{
			if(index == 0) write_buffer[dest + 0] = u64_mul(read_buffer[src].x, read_buffer[src].y);
			if(index == 1) write_buffer[dest + 1] = u32_mul(read_buffer[src].x, read_buffer[src].y);
			if(index == 2) write_buffer[dest + 2] = umulExt(read_buffer[src].x, read_buffer[src].y);
			index -= 3;
			dest += 3;
			src += 1;
		}
		
		// i32 * i32
		{
			if(index == 0) write_buffer[dest + 0] = uvec2(i64_mul(read_buffer[src].x, read_buffer[src].y));
			if(index == 1) write_buffer[dest + 1] = uvec2(i32_mul(read_buffer[src].x, read_buffer[src].y));
			if(index == 2) write_buffer[dest + 2] = uvec2(imulExt(read_buffer[src].x, read_buffer[src].y));
			index -= 3;
			dest += 3;
			src += 1;
		}
		{
			if(index == 0) write_buffer[dest + 0] = uvec2(i64_mul(read_buffer[src].x, read_buffer[src].y));
			if(index == 1) write_buffer[dest + 1] = uvec2(i32_mul(read_buffer[src].x, read_buffer[src].y));
			if(index == 2) write_buffer[dest + 2] = uvec2(imulExt(read_buffer[src].x, read_buffer[src].y));
			index -= 3;
			dest += 3;
			src += 1;
		}
		{
			if(index == 0) write_buffer[dest + 0] = uvec2(i64_mul(read_buffer[src].x, read_buffer[src].y));
			if(index == 1) write_buffer[dest + 1] = uvec2(i32_mul(read_buffer[src].x, read_buffer[src].y));
			if(index == 2) write_buffer[dest + 2] = uvec2(imulExt(read_buffer[src].x, read_buffer[src].y));
			index -= 3;
			dest += 3;
			src += 1;
		}
		
		// u32 + u32
		{
			if(index == 0) write_buffer[dest + 0] = uaddExt(read_buffer[src + 0].x, read_buffer[src + 0].y);
			if(index == 1) write_buffer[dest + 1] = uaddExt(read_buffer[src + 1].x, read_buffer[src + 1].y);
			index -= 2;
			dest += 2;
			src += 2;
		}
		
		// u32 - u32
		{
			if(index == 0) write_buffer[dest + 0] = usubExt(read_buffer[src + 0].x, read_buffer[src + 0].y);
			if(index == 1) write_buffer[dest + 1] = usubExt(read_buffer[src + 1].x, read_buffer[src + 1].y);
			index -= 2;
			dest += 2;
			src += 2;
		}
	}
	
#endif
