// Tellusim Fragment Application Template

/*
 */
var Module = {
	preRun: () => {
		Module.FS.createPreloadedFile('/', 'main.shader', 'main.shader', true, false);
	},
};

/*
 */
Tellusim(Module).then(ts => {
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
		
		// create resources
		if(!this.create_resources()) return false;
		
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
		
		// fragment uniform parameters
		let parameters_buffer = new ArrayBuffer(8);
		let parameters = new Float32Array(parameters_buffer);
		parameters.set([ this.window.width / this.window.height ]);
		parameters.set([ ts.Time.seconds() ], 1);
		
		// draw fullscreen triangle
		command.setPipeline(this.pipeline);
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
	
	// helper function to create and load resources
	create_resources() {
		
		let ts = this.ts;
		
		// create pipeline
		this.pipeline = this.device.createPipeline();
		this.pipeline.setUniformMask(0, ts.Shader.Mask.Fragment);
		this.pipeline.setColorFormat(this.window.color_format);
		this.pipeline.setDepthFormat(this.window.depth_format);
		if(!this.pipeline.loadShaderGLSL(ts.Shader.Type.Vertex, 'main.shader', 'VERTEX_SHADER=1')) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_resources(): can\'t load Vertex shader\n');
			return false;
		}
		if(!this.pipeline.loadShaderGLSL(ts.Shader.Type.Fragment, "main.shader", 'FRAGMENT_SHADER=1')) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_resources(): can\'t load Fragment shader\n');
			return false;
		}
		if(!this.pipeline.create()) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_resources(): can\'t create Pipeline\n');
			return false;
		}
		
		// your resource creation code here
		
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
