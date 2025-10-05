// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#include <common/common.h>
#include <math/TellusimMath.h>
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
	
	// print device info
	TS_LOGF(Message, "%s (%s)\n", device.getName().get(), device.getPlatformName());
	
	// create kernel
	Kernel kernel = device.createKernel().setStorages(2);
	if(!kernel.loadShaderGLSL("main.shader", "COMPUTE_SHADER=1")) return 1;
	if(!kernel.create()) return 1;
	
	// create buffers
	Array<uint32_t> read_buffer_data(1024, 0u);
	Array<uint32_t> write_buffer_data(1024, 0u);
	
	// u32 * u32
	read_buffer_data[0] = 0x89abcdef;
	read_buffer_data[1] = 0xfedcba98;
	
	// i32 * i32
	read_buffer_data[2] = f32u32(-(int32_t)0x89abcdef).u;
	read_buffer_data[3] = f32u32( (int32_t)0xfedcba98).u;
	read_buffer_data[4] = f32u32(-(int32_t)0x89abcdef).u;
	read_buffer_data[5] = f32u32(-(int32_t)0xfedcba98).u;
	read_buffer_data[6] = f32u32(-1).u;
	read_buffer_data[7] = f32u32(-1).u;
	
	// u32 + u32
	read_buffer_data[8] = 0x89abcdef;
	read_buffer_data[9] = 0xfedcba98;
	read_buffer_data[10] = 0x0fedcba9;
	read_buffer_data[11] = 0x89abcdef;
	
	// u32 - u32
	read_buffer_data[12] = 0x89abcdef;
	read_buffer_data[13] = 0xfedcba98;
	read_buffer_data[14] = 0xfedcba98;
	read_buffer_data[15] = 0x89abcdef;
	
	Buffer read_buffer = device.createBuffer(Buffer::FlagStorage | Buffer::FlagSource, read_buffer_data.get(), read_buffer_data.bytes());
	Buffer write_buffer = device.createBuffer(Buffer::FlagStorage | Buffer::FlagSource, write_buffer_data.get(), write_buffer_data.bytes());
	if(!read_buffer || !write_buffer) return 1;
	
	{
		// create command list
		Compute compute = device.createCompute();
		
		// dispatch kernel
		compute.setKernel(kernel);
		compute.setStorageBuffer(0, read_buffer);
		compute.setStorageBuffer(1, write_buffer);
		compute.dispatch(64);
	}
	
	// flush context
	context.flush();
	
	// get write buffer
	if(!device.getBuffer(write_buffer, write_buffer_data.get(), write_buffer_data.bytes())) return 1;
	const uint32_t *dest = write_buffer_data.get();
	const uint32_t *src = read_buffer_data.get();
	
	// u32 * u32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", (uint64_t)src[0] * (uint64_t)src[1]);
	TS_LOGF(Message, "u64_mul: 0x%08x%08x\n", dest[0], dest[1]);
	TS_LOGF(Message, "u32_mul: 0x%08x%08x\n", dest[2], dest[3]);
	TS_LOGF(Message, "umulExt: 0x%08x%08x\n", dest[4], dest[5]);
	dest += 6;
	src += 2;
	
	// i32 * i32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", (int64_t)f32u32(src[0]).i * (int64_t)f32u32(src[1]).i);
	TS_LOGF(Message, "i64_mul: 0x%08x%08x\n", dest[0], dest[1]);
	TS_LOGF(Message, "i32_mul: 0x%08x%08x\n", dest[2], dest[3]);
	TS_LOGF(Message, "imulExt: 0x%08x%08x\n", dest[4], dest[5]);
	dest += 6;
	src += 2;
	
	// i32 * i32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", (int64_t)f32u32(src[0]).i * (int64_t)f32u32(src[1]).i);
	TS_LOGF(Message, "i64_mul: 0x%08x%08x\n", dest[0], dest[1]);
	TS_LOGF(Message, "i32_mul: 0x%08x%08x\n", dest[2], dest[3]);
	TS_LOGF(Message, "imulExt: 0x%08x%08x\n", dest[4], dest[5]);
	dest += 6;
	src += 2;
	
	// i32 * i32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", (int64_t)f32u32(src[0]).i * (int64_t)f32u32(src[1]).i);
	TS_LOGF(Message, "i64_mul: 0x%08x%08x\n", dest[0], dest[1]);
	TS_LOGF(Message, "i32_mul: 0x%08x%08x\n", dest[2], dest[3]);
	TS_LOGF(Message, "imulExt: 0x%08x%08x\n", dest[4], dest[5]);
	dest += 6;
	src += 2;
	
	// u32 + u32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", (uint64_t)src[0] + (uint64_t)src[1]);
	TS_LOGF(Message, "uaddExt: 0x%08x%08x\n", dest[0], dest[1]);
	dest += 2;
	src += 2;
	
	// u32 + u32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", (uint64_t)src[0] + (uint64_t)src[1]);
	TS_LOGF(Message, "uaddExt: 0x%08x%08x\n", dest[0], dest[1]);
	dest += 2;
	src += 2;
	
	// u32 - u32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", ((uint64_t)src[0] - (uint64_t)src[1]) & (uint64_t)0x00000001ffffffffull);
	TS_LOGF(Message, "usubExt: 0x%08x%08x\n", dest[0], dest[1]);
	dest += 2;
	src += 2;
	
	// u32 - u32
	Log::print("\n");
	TS_LOGF(Message, "    ref: 0x%016" TS_FORMAT_64 "x\n", ((uint64_t)src[0] - (uint64_t)src[1]) & (uint64_t)0x00000001ffffffffull);
	TS_LOGF(Message, "usubExt: 0x%08x%08x\n", dest[0], dest[1]);
	dest += 2;
	src += 2;
	
	// finish context
	context.finish();
	
	return 0;
}
