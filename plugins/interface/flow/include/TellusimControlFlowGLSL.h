// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#ifndef __TELLUSIM_PLUGINS_INTERFACE_CONTROL_FLOW_GLSL_H__
#define __TELLUSIM_PLUGINS_INTERFACE_CONTROL_FLOW_GLSL_H__

#include "TellusimControlFlow.h"

/*
 */
namespace Tellusim {
	
	/*
	 */
	class ControlFlowGLSL : public ControlFlow {
			
		public:
			
			/// ControlFlow constructor
			explicit ControlFlowGLSL(Control *parent, const char *variant = nullptr, uint32_t version = 0);
			virtual ~ControlFlowGLSL();
			
			/// create flow
			virtual bool create(Control *controls_root = nullptr, Control *tooltip_root = nullptr);
			
			/// create source for the node
			String getSource(uint32_t node);
			
			/// type name
			virtual const char *getGLSLTypeName(uint32_t type) const;
			
			/// flow colors
			TS_INLINE const Color &getGLSLColor() const { return glsl_color; }
			TS_INLINE const Color &getToolColor() const { return tool_color; }
			
			/// glsl types
			TS_INLINE uint32_t getAnyType() const { return any_type; }
			TS_INLINE uint32_t getIntType() const { return int_type; }
			TS_INLINE uint32_t getFloatType() const { return float_type; }
			TS_INLINE uint32_t getVec2Type() const { return vec2_type; }
			TS_INLINE uint32_t getVec3Type() const { return vec3_type; }
			TS_INLINE uint32_t getVec4Type() const { return vec4_type; }
			TS_INLINE uint32_t getMat2Type() const { return mat2_type; }
			TS_INLINE uint32_t getMat3Type() const { return mat3_type; }
			TS_INLINE uint32_t getMat4Type() const { return mat4_type; }
			TS_INLINE uint32_t getMat32Type() const { return mat32_type; }
			TS_INLINE uint32_t getMat43Type() const { return mat43_type; }
			
			/// color callback
			using ColorChangedCallback = Function<void(Color color, bool is_changed)>;
			using ColorCallback = Function<bool(uint32_t node, Color color, ColorChangedCallback func)>;
			void setColorCallback(const ColorCallback &func) { color_func = func; }
			TS_INLINE const ColorCallback &getColorCallback() const { return color_func; }
			
		protected:
			
			/// glsl types
			TS_INLINE bool is_vector_type(uint32_t type) const { return (type == vec2_type || type == vec3_type || type == vec4_type); }
			TS_INLINE bool is_matrix_type(uint32_t type) const { return (type == mat2_type || type == mat3_type || type == mat4_type || type == mat32_type || type == mat43_type); }
			
			/// create glsl
			void create_types();
			void create_protos();
			void create_tools();
			
			/// create editable slider
			ControlSlider create_slider(Control *root, const char *name = nullptr, uint32_t digits = 2, float64_t value = 0.0, float64_t min = 0.0, float64_t max = 1.0, float32_t width = 128.0f);
			static void expand_slider(ControlSlider &slider);
			
			/// create combo from items
			template <class Value, uint32_t Size> static ControlCombo create_combo(Control *root, const char * const (&items)[Size], Value (&values)[Size]);
			
			/// type callback
			static bool any_callback(ControlFlow *flow, uint32_t node, uint32_t input, uint32_t output_node, uint32_t output_index);
			
			/// flow state
			static bool set_state(ControlCombo &combo, const String &name);
			
			/// matrix types
			uint32_t get_matrix_t_type(uint32_t type) const;
			uint32_t get_matrix_r_type(uint32_t type) const;
			uint32_t get_matrix_row_type(uint32_t type) const;
			uint32_t get_num_matrix_rows(uint32_t type) const;
			
			float32_t matrix_width = 96.0f;		// matrix slider width
			
			uint32_t any_type = Maxu32;			// any type
			
			uint32_t int_type = Maxu32;			// scalar types
			uint32_t float_type = Maxu32;
			
			uint32_t vec2_type = Maxu32;		// vector types
			uint32_t vec3_type = Maxu32;
			uint32_t vec4_type = Maxu32;
			
			uint32_t mat2_type = Maxu32;		// matrix types
			uint32_t mat3_type = Maxu32;
			uint32_t mat4_type = Maxu32;
			uint32_t mat32_type = Maxu32;
			uint32_t mat43_type = Maxu32;
			
			ColorCallback color_func;			// color callback
			
			Color glsl_color = Color(1.0f, 1.0f, 1.0f, 0.75f);
			Color tool_color = Color(0.3f, 0.6f, 1.0f, 0.75f);
	};
}

#endif /* __TELLUSIM_PLUGINS_INTERFACE_CONTROL_FLOW_GLSL_H__ */
