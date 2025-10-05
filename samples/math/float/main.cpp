// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#include <core/TellusimLog.h>
#include <math/TellusimFloat.h>

/*
 */
using namespace Tellusim;

/*
 */
int32_t main(int32_t argc, char **argv) {
	
	{
		using Tellusim::pow;
		
		for(int32_t i = -9; i <= 16; i++) {
			for(int32_t j = 2; j <= 3; j++) {
				float32_t f = pow(2.0f, (float32_t)i) * ((float32_t)j / 2.0f);
				TS_LOGF(Message, "%f %f %f %f\n", f, float16_t(f).get(), float21_t(f).get(), float24_t(f).get());
			}
		}
	}
	
	return 0;
}
