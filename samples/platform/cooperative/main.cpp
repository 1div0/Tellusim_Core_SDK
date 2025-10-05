// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#include <common/common.h>
#include <math/TellusimNumerical.h>
#include <platform/TellusimDevice.h>
#include <platform/TellusimKernel.h>
#include <platform/TellusimCompute.h>
#include <platform/TellusimContext.h>

/*
 */
using namespace Tellusim;

/*
 */
int32_t main(int32_t argc, char **argv) {
	
	// create app
	App app(argc, argv);
	if(!app.create()) return 1;
	
	// create context
	Context context(app.getPlatform(), app.getDevice());
	if(!context || !context.create()) return 1;
	
	// create device
	Device device(context);
	if(!device) return 1;
	
	// check compute shader support
	if(!device.hasShader(Shader::TypeCompute)) {
		TS_LOG(Error, "compute shader is not supported\n");
		return 0;
	}
	
	// check cooperative matrix support
	if(!device.getFeatures().matrix16f16) {
		TS_LOG(Error, "cooperative matrix is not supported\n");
		return 0;
	}
	
	// print device info
	TS_LOGF(Message, "%s (%s)\n", device.getName().get(), device.getPlatformName());
	
	// matrix size
	constexpr uint32_t N = 16;
	
	// create kernel
	Kernel kernel = device.createKernel().setStorages(1);
	if(!kernel.loadShaderGLSL("main.shader", "COMPUTE_SHADER=1; M=%u; N=%u; K=%u", N, N, N)) return 1;
	if(!kernel.create()) return 1;
	
	// create matrices
	Array<float16_t> data;
	for(uint32_t i = 0; i < N * N; i++) {
		uint32_t j = i % N;
		uint32_t k = i / N;
		float32_t value = (float32_t)(N + N - j - k) / (N + N);
		if(j < k) value = -value;
		data.append(float16_t(value));
	}
	for(uint32_t i = 0; i < N * N * 3; i++) {
		data.append(float16_t::zero);
	}
	
	// inverse matrix
	MatrixNxM<float64_t, N, N> a(MatrixNxM<float16_t, N, N>(data.get() + N * N * 0));
	MatrixNxM<float64_t, N, N> b = LU::inverse(a);
	MatrixNxM<float64_t, N, N> c = -(a * b);
	
	MatrixNxM<float16_t, N, N>(a).get(data.get() + N * N * 0);
	MatrixNxM<float16_t, N, N>(b).get(data.get() + N * N * 1);
	MatrixNxM<float16_t, N, N>(c).get(data.get() + N * N * 2);
	
	// create buffer
	Buffer buffer = device.createBuffer(Buffer::FlagStorage | Buffer::FlagSource, data.get(), data.bytes());
	if(!buffer) return 1;
	
	{
		// create command list
		Compute compute = device.createCompute();
		
		// run kernel
		compute.setKernel(kernel);
		compute.setStorageBuffer(0, buffer);
		compute.dispatch(1);
	}
	
	// flush context
	context.flush();
	
	// get buffer
	if(!device.getBuffer(buffer, data.get())) return 1;
	
	// print result
	const char *names[] = { "a", "b", "c", "d" };
	for(uint32_t i = 0, m = 0; i < N * N * 4; m++) {
		Log::printf("%s:\n", names[m]);
		for(uint32_t j = 0; j < N; j++) {
			String row;
			for(uint32_t k = 0; k < N; k++, i++) {
				if(row) row += " ";
				row += String::format("%7.4f", float16_t(data[i]).get());
			}
			row += "\n";
			Log::print(row.get());
		}
		Log::print("\n");
	}
	
	// reference result
	a = MatrixNxM<float64_t, N, N>(MatrixNxM<float16_t, N, N>(data.get() + N * N * 0));
	b = MatrixNxM<float64_t, N, N>(MatrixNxM<float16_t, N, N>(data.get() + N * N * 1));
	c = MatrixNxM<float64_t, N, N>(MatrixNxM<float16_t, N, N>(data.get() + N * N * 2));
	MatrixNxM<float64_t, N, N> d = a * b + c;
	
	Log::print("ref:\n");
	for(uint32_t j = 0; j < N; j++) {
		String row;
		for(uint32_t k = 0; k < N; k++) {
			if(row) row += " ";
			row += String::format("%7.4f", d[k][j]);
		}
		row += "\n";
		Log::print(row.get());
	}
	Log::print("\n");
	
	// check errors
	device.check();
	
	// finish context
	context.finish();
	
	return 0;
}
