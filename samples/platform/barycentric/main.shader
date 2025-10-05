// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#version 420 core

/*
 */
#if VERTEX_SHADER
	
	layout(location = 0) in vec4 in_position;
	
	/*
	 */
	void main() {
		
		gl_Position = in_position;
	}
	
#elif FRAGMENT_SHADER
	
	layout(location = 0) out vec4 out_color;
	
	/*
	 */
	void main() {
		
		out_color = vec4(gl_BaryCoordEXT, 1.0f);
	}
	
#endif
