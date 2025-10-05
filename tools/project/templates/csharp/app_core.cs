// Tellusim Core SDK Application Template

#define RENDER_TRIANGLE

using System;
using Tellusim;

/*
 */
class Application {
	
	// constructor initializing the application with command-line arguments
	public Application(string[] args) {
		app = new App(args);
	}
	
	// create function responsible for initialization
	public bool create() {
		
		// create app
		if(!app.create()) {
			Log.print(Log.Level.Error, "Application::create(): can't create App\n");
			return false;
		}
		
		// create window
		if(!create_window()) return false;
		
		// create pipeline
		#if RENDER_TRIANGLE
			if(!create_pipeline()) return false;
		#endif
		
		// additional initialization code can go here
		
		return true;
	}
	
	// update function for logic updates
	public bool update() {
		
		// your update logic here
		
		return true;
	}
	
	// render function to handle the drawing commands
	public bool render(Command command) {
		
		// render pipeline
		#if RENDER_TRIANGLE
			command.setPipeline(pipeline);
			float angle = (float)Time.seconds() * 64.0f;
			float iaspect = (float)window.getHeight() / window.getWidth();
			command.setUniform(0, Matrix4x4f.rotateZ(angle) * Matrix4x4f.scale(iaspect, 1.0f, 1.0f));
			command.drawArrays(3);
		#endif
		
		// your rendering code here
		
		return true;
	}
	
	// main run loop
	public bool run() {
		
		// run application
		window.run((IntPtr data) => {
			return render_window();
		});
		
		// finish context
		window.finish();
		
		// keep window alive
		window.unacquirePtr();
		
		return true;
	}
	
	// helper function to create and initialize the Window
	private bool create_window() {
		
		// create Window
		window = new Window(app.getPlatform(), app.getDevice());
		if(!window || !window.setSize(app.getWidth(), app.getHeight()) ||
			!window.create(window.getPlatformName() + " C# @NAME@") || !window.setHidden(false)) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Window\n");
			return false;
		}
		
		// set Window callbacks
		window.setKeyboardPressedCallback((uint key, uint code, IntPtr data) => {
			if(key == (uint)Window.Key.F4 && window.getKeyboardKey((uint)Window.Key.Alt)) window.stop();
			if(key == (uint)Window.Key.Esc) window.stop();
		});
		window.setCloseClickedCallback((IntPtr data) => { window.stop(); });
		
		// create Device
		device = new Device(window);
		if(!device) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Device\n");
			return false;
		}
		
		// create Target
		target = device.createTarget(window);
		if(!target) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Target\n");
			return false;
		}
		
		return true;
	}
	
	// helper function to create the Pipeline
	#if RENDER_TRIANGLE
		private bool create_pipeline() {
			
			// create and configure the rendering pipeline
			pipeline = device.createPipeline();
			pipeline.setUniformMask(0, Shader.Mask.Vertex);
			pipeline.setColorFormat(window.getColorFormat());
			pipeline.setDepthFormat(window.getDepthFormat());
			pipeline.setDepthFunc(Pipeline.DepthFunc.Always);
			
			// vertex Shader that defines a triangle and transforms its position
			if(!pipeline.createShaderGLSL(Shader.Type.Vertex, @"
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
			")) return false;
			
			// fragment shader
			if(!pipeline.createShaderGLSL(Shader.Type.Fragment, @"
				layout(location = 0) in vec4 s_color;
				layout(location = 0) out vec4 out_color;
				void main() {
					out_color = s_color;
				}
			")) return false;
			
			if(!pipeline.create()) return false;
			
			target.setClearColor(new Color(0.2f));
			
			return true;
		}
	#endif
	
	// helper function to render the Window
	private bool render_window() {
		
		// update events
		Window.update();
		
		// update application
		if(!update()) return false;
		
		// render window
		if(!window.render()) return false;
		
		// window target
		target.begin();
		{
			Command command = device.createCommand(target);
			
			// render application
			if(!render(command)) {
				target.end();
				return false;
			}
			
			command.destroyPtr();
		}
		target.end();
		
		// present window
		if(!window.present()) return false;
		
		// check errors
		if(!device.check()) return false;
		
		// memory
		GC.Collect();
		GC.WaitForPendingFinalizers();
		
		return true;
	}
	
	private App app;						// Application interface
	private Window window = Window.Null();	// Window interface
	private Device device = Device.Null();	// Device interface
	private Target target = Target.Null();	// Target interface
	
	#if RENDER_TRIANGLE
		private Pipeline pipeline = Pipeline.Null();
	#endif
}

/*
 */
class AppCore {
	
	/*
	 */
	[STAThread]
	static void Main(string[] args) {
		
		// instantiate the application
		var application = new Application(args);
		
		// create the application
		if(!application.create()) return;
		
		// run the application
		if(!application.run()) return;
	}
}
