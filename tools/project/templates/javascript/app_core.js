// Tellusim Core SDK Application Template

/*
 */
Tellusim().then(ts => {
	ts.run(main);
});

/*
 */
class Application {
	
	// constructor initializing the application
	constructor(ts, app) {
		this.ts = ts;
		this.app = app;
	}
	
	// create function responsible for initialization
	create() {
		
		// create window
		if(!this.create_window()) return false;
		
		// create pipeline
		if(!this.create_pipeline()) return false;
		
		// additional initialization code can go here
		
		return true;
	}
	
	// update function for logic updates
	update() {
		
		// your update logic here
		
		return true;
	}
	
	// render function to handle the drawing commands
	render(command) {
		
		let ts = this.ts;
		
		// render pipeline
		command.setPipeline(this.pipeline);
		let angle = ts.Time.seconds() * 64.0;
		let iaspect = this.window.height / this.window.width;
		let parameters_buffer = new ArrayBuffer(64);
		let parameters = new Float32Array(parameters_buffer);
		parameters.set(ts.Matrix4x4f.rotateZ(angle).mul(ts.Matrix4x4f.scale(iaspect, 1.0, 1.0)).getArray());
		command.setUniform(0, parameters);
		command.drawArrays(3);
		
		// your rendering code here
		
		return true;
	}
	
	// main run loop
	run() {
		
		// run application
		this.window.run(() => {
			return this.render_window();
		});
		
		// finish context
		this.window.finish();
		
		return true;
	}
	
	// helper function to create and initialize the Window
	create_window() {
		
		let ts = this.ts;
		
		// create Window
		this.window = new ts.Window(this.app.platform, this.app.device);
		if(!this.window.isValidPtr() || !this.window.setSize(this.app.width, this.app.height) ||
			!this.window.create(this.window.platform_name + ' JavaScript @NAME@') || !this.window.setHidden(false)) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_window(): can\'t create Window\n');
			return false;
		}
		
		// set Window callbacks
		this.window.setKeyboardPressedCallback((key, code) => {
			if(key == ts.Window.Key.Esc) this.window.stop();
		});
		this.window.setCloseClickedCallback(() => {
			this.window.stop();
		});
		
		// create Device
		this.device = new ts.Device(this.window);
		if(!this.device.isValidPtr()) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_window(): can\'t create Device\n');
			return false;
		}
		
		// create Target
		this.target = this.device.createTarget(this.window);
		if(!this.target.isValidPtr()) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_window(): can\'t create Target\n');
			return false;
		}
		
		return true;
	}
	
	// helper function to create the Pipeline
	create_pipeline() {
		
		let ts = this.ts;
		
		// create and configure the rendering pipeline
		this.pipeline = this.device.createPipeline();
		this.pipeline.setUniformMask(0, ts.Shader.Mask.Vertex);
		this.pipeline.setColorFormat(this.window.color_format);
		this.pipeline.setDepthFormat(this.window.depth_format);
		this.pipeline.setDepthFunc(ts.Pipeline.DepthFunc.Always);
		
		// vertex Shader that defines a triangle and transforms its position
		if(!this.pipeline.createShaderGLSL(ts.Shader.Type.Vertex, `
			layout(std140, binding = 0) uniform Parameters { mat4 transform; };
			layout(location = 0) out vec4 s_color;
			void main() {
				float height = 2.0f * sqrt(5.0f);
				vec2 position = vec2(0.0f, height);
				if(gl_VertexIndex == 0) position = vec2(-sqrt(15.0f), -sqrt(5.0f));
				if(gl_VertexIndex == 1) position = vec2( sqrt(15.0f), -sqrt(5.0f));
				position /= height;
				gl_Position = transform * vec4(position, 0.0f, 1.0f);
				s_color = max(vec4(position + 0.5f, -position.x - position.y, 1.0f), vec4(0.0f));
			}
		`)) return false;
		
		// fragment shader
		if(!this.pipeline.createShaderGLSL(ts.Shader.Type.Fragment, `
			layout(location = 0) in vec4 s_color;
			layout(location = 0) out vec4 out_color;
			void main() {
				out_color = s_color;
			}
		`)) return false;
		
		if(!this.pipeline.create()) return false;
		
		this.target.setClearColor(new ts.Color(0.2));
		
		return true;
	}
	
	// helper function to render the Window
	render_window() {
		
		let ts = this.ts;
		
		// update events
		ts.Window.update();
		
		// update application
		if(!this.update()) return false;
		
		// render window
		if(!this.window.render()) return false;
		
		// window target
		this.target.begin();
		{
			let command = this.device.createCommand(this.target);
			
			// render application
			if(!this.render(command)) {
				this.target.end();
				return false;
			}
			
			command.destroyPtr();
		}
		this.target.end();
		
		// present window
		if(!this.window.present()) return false;
		
		// check errors
		if(!this.device.check()) return false;
		
		return true;
	}
}

/*
 */
function main(ts, app) {
	
	// instantiate the application
	let application = new Application(ts, app);
	
	// create the application
	if(!application.create()) return;
	
	// run the application
	if(!application.run()) return;
}
