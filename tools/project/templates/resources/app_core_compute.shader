// Tellusim Compute Application Template

#version 430 core

/*
 */
#if COMPUTE_SHADER
	
	layout(local_size_x = 8, local_size_y = 8) in;
	
	layout(std140, binding = 0) uniform ComputeParameters {
		float time;
	};
	
	layout(binding = 0, set = 1, rgba8) uniform writeonly image2D out_surface;
	
	/*
	 */
	vec3 tellusim(vec3 color, vec2 r, float scale) {
		
		r.y = r.y * 0.86f - abs(r.x) * 0.5f;
		
		float d0 = 1.0f;
		d0 = min(d0, max(abs(r.x) + abs(r.y) - 0.21f, -r.y));
		d0 = min(d0, length(max(abs(r + vec2( 0.105f, 0.105f)) - 0.105f, 0.0f)));
		d0 = min(d0, length(max(abs(r + vec2(-0.105f, 0.105f)) - 0.105f, 0.0f)));
		
		float d1 = 1.0f;
		d1 = min(d1, max(abs(r.x) + abs(r.y) - 0.15f, 0.0f));
		d1 = max(d1, max(min(r.x - 0.04f, r.y - 0.092f), -r.y));
		d1 = max(d1, max(min(r.x - 0.091f, r.y - 0.041f), -r.y));
		
		float d2 = 1.0f;
		d2 = min(d2, length(max(abs(r + vec2( 0.075f, 0.03f)) - vec2(0.075f, 0.03f), 0.0f)));
		d2 = min(d2, length(max(abs(r + vec2( 0.08f, 0.10f)) - vec2(0.03f, 0.05f), 0.0f)));
		d2 = min(d2, max(length(max(abs(r + vec2(-0.025f, 0.105f)) - vec2(0.025f, 0.045f), 0.0f)), max(-r.x - r.y - 0.1f, 0.0f)));
		
		float d3 = 1.0f;
		d3 = min(d3, length(max(abs(r + vec2(-0.02f, -0.101f)) - vec2(0.02f, 0.008f), 0.0f)));
		d3 = min(d3, max(length(max(abs(vec2(r.x, r.y - r.x) + vec2( 0.03f, -0.101f)) - vec2(0.03f, 0.008f), 0.0f)), - r.y + 0.06f));
		d3 = min(d3, max(length(max(abs(r + vec2(-0.046f, -0.05f)) - vec2(0.045f, 0.008f), 0.0f)), -r.x - r.y + 0.06f));
		d3 = min(d3, max(length(max(abs(r + vec2( 0.025f, 0.105f)) - vec2(0.025f, 0.045f), 0.0f)), max( r.x - r.y - 0.1f, 0.0f)));
		d3 = min(d3, length(max(abs(r + vec2(-0.075f, 0.03f)) - vec2(0.075f, 0.03f), 0.0f)));
		d3 = min(d3, length(max(abs(r + vec2(-0.08f, 0.10f)) - vec2(0.03f, 0.05f), 0.0f)));
		
		color = mix(color, vec3(0.0f), max(1.0f - d0 * scale, 0.0f));
		color = mix(color, vec3(0.22f, 0.22f, 0.22f), max(1.0f - d1 * scale, 0.0f));
		color = mix(color, vec3(0.15f, 0.15f, 0.15f), max(1.0f - d2 * scale, 0.0f));
		color = mix(color, vec3(0.97f, 0.12f, 0.28f), max(1.0f - d3 * scale, 0.0f));
		
		return color;
	}
	
	/*
	 */
	void main() {
		
		ivec2 size = imageSize(out_surface);
		vec2 texcoord = (vec2(gl_GlobalInvocationID.xy) + 0.5f) / vec2(size);
		
		// replace with your shading function
		vec3 color = vec3(texcoord, 1.0f - texcoord.x - texcoord.y);
		color = sin(color * vec3(5.0f, 6.0f, 7.0f) + time) * 0.5f + 0.5f;
		color = tellusim(color, (texcoord - 0.5f) * vec2(float(size.x) / float(size.y), 1.0f), float(size.y));
		
		imageStore(out_surface, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0f));
	}
	
#elif VERTEX_SHADER
	
	layout(location = 0) out vec2 s_texcoord;
	
	/*
	 */
	void main() {
		
		vec2 texcoord = vec2(0.0f);
		if(gl_VertexIndex == 0) texcoord.x = 2.0f;
		if(gl_VertexIndex == 2) texcoord.y = 2.0f;
		
		gl_Position = vec4(texcoord * 2.0f - 1.0f, 0.0f, 1.0f);
		
		#if CLAY_VK
			texcoord.y = 1.0f - texcoord.y;
		#endif
		
		s_texcoord = texcoord;
	}
	
#elif FRAGMENT_SHADER
	
	layout(binding = 0, set = 0) uniform texture2D in_texture;
	
	layout(location = 0) in vec2 s_texcoord;
	
	layout(location = 0) out vec4 out_color;
	
	/*
	 */
	void main() {
		
		ivec2 size = textureSize(in_texture, 0);
		ivec2 texcoord = ivec2(vec2(size) * s_texcoord);
		
		out_color = texelFetch(in_texture, texcoord, 0);
	}
	
#endif
