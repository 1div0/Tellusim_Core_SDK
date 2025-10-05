// Tellusim Fragment Application Template

using System;
using Tellusim;
using System.Runtime.InteropServices;

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
		
		// create resources
		if(!create_resources()) return false;
		
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
		
		// fragment uniform parameters
		FragmentParameters parameters = new FragmentParameters();
		parameters.aspect = (float)window.getWidth() / (float)window.getHeight();
		parameters.time = (float)Time.seconds();
		
		// draw fullscreen triangle
		command.setPipeline(pipeline);
		command.setUniform(0, parameters);
		command.drawArrays(3);
		
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
	
	// helper function to create and load resources
	private bool create_resources() {
		
		// create pipeline
		pipeline = device.createPipeline();
		pipeline.setUniformMask(0, Shader.Mask.Fragment);
		pipeline.setColorFormat(window.getColorFormat());
		pipeline.setDepthFormat(window.getDepthFormat());
		if(!pipeline.loadShaderGLSL(Shader.Type.Vertex, "main.shader", "VERTEX_SHADER=1")) {
			Log.print(Log.Level.Error, "Application::create_resources(): can't load Vertex shader\n");
			return false;
		}
		if(!pipeline.loadShaderGLSL(Shader.Type.Fragment, "main.shader", "FRAGMENT_SHADER=1")) {
			Log.print(Log.Level.Error, "Application::create_resources(): can't load Fragment shader\n");
			return false;
		}
		if(!pipeline.create()) {
			Log.print(Log.Level.Error, "Application::create_resources(): can't create Pipeline\n");
			return false;
		}
		
		// your resource creation code here
		
		return true;
	}
	
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
	
	// fragment uniform parameters
	[StructLayout(LayoutKind.Sequential)]
	public struct FragmentParameters {
		public float aspect;
		public float time;
	}
	
	private App app;								// Application interface
	private Window window = Window.Null();			// Window interface
	private Device device = Device.Null();			// Device interface
	private Target target = Target.Null();			// Target interface
	
	private Pipeline pipeline = Pipeline.Null();	// Pipeline interface
}

/*
 */
class AppFragment {
	
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
