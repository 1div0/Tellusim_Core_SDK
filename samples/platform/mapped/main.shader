// Clay header

#version 430 core

/*
 */
#if VERTEX_SHADER
	
	layout(location = 0) in vec4 in_position;
	layout(location = 1) in vec2 in_texcoord;
	
	layout(location = 0) out vec2 s_texcoord;
	
	/*
	 */
	void main() {
		
		gl_Position = in_position;
		
		s_texcoord = in_texcoord;
	}
	
#elif FRAGMENT_SHADER
	
	layout(std430, binding = 0) readonly buffer mapped_buffer { uint mapped_data[]; };
	
	layout(location = 0) in vec2 s_texcoord;
	
	layout(location = 0) out vec4 out_color;
	
	/*
	 */
	void main() {
		
		ivec2 texcoord = ivec2(s_texcoord * 512.0f);
		
		uint color = mapped_data[512 * texcoord.y + texcoord.x];
		
		out_color = unpackUnorm4x8(color);
	}
	
#endif
