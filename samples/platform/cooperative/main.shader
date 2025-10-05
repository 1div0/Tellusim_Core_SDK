// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#version 430 core

/*
 */
#if COMPUTE_SHADER
	
	layout(local_size_x = 32) in;
	
	layout(std430, binding = 0) buffer DataBuffer { float16_t data[]; };
	
	coopmat<float16_t, gl_ScopeSubgroup, M, N, gl_MatrixUseAccumulator> c;
	coopmat<float16_t, gl_ScopeSubgroup, M, N, gl_MatrixUseAccumulator> d;
	
	/*
	 */
	void main() {
		
		coopmat<float16_t, gl_ScopeSubgroup, M, K, gl_MatrixUseA> a;
		coopmat<float16_t, gl_ScopeSubgroup, K, N, gl_MatrixUseB> b;
		
		coopMatLoad(a, data, M * M * 0, K, gl_CooperativeMatrixLayoutRowMajor);
		coopMatLoad(b, data, M * M * 1, N, gl_CooperativeMatrixLayoutRowMajor);
		coopMatLoad(c, data, M * M * 2, N, gl_CooperativeMatrixLayoutRowMajor);
		
		d = coopMatMulAdd(a, b, c);
		
		coopMatStore(d, data, M * M * 3, N, gl_CooperativeMatrixLayoutRowMajor);
	}
	
#endif
