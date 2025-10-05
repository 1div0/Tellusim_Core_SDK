// Tellusim Canvas Application Template

using System;
using Tellusim;
using System.Collections.Generic;

/*
 */
class Application {
	
	// constructor initializing the application with command-line arguments
	public Application(string[] args) {
		app = new App(args);
		ellipses = new List<CanvasEllipse>();
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
		
		// update canvas elements
		float hheight = canvas.getHeight() * 0.5f;
		for(int i = 0; i < ellipses.Count; i++) {
			float angle = Base.Pi2 * i / ellipses.Count + (float)Time.seconds();
			float x = canvas.getWidth() * (1 + i) / (ellipses.Count + 1);
			float y0 = hheight + (float)(Math.Cos(angle * 2.0f - 0.2f) * Math.Sin(angle - 0.1f)) * hheight * 0.9f;
			float y1 = hheight + (float)(Math.Cos(angle * 2.0f + 0.2f) * Math.Sin(angle + 0.1f)) * hheight * 0.9f;
			ellipses[i].setColor(Color.hsv(angle * Base.Rad2Deg + y0 * 0.5f, 1.0f, 1.0f));
			ellipses[i].setRadius((float)Math.Sqrt(20.0f * 20.0f + (y1 - y0) * (y1 - y0) * 0.25f));
			ellipses[i].setPosition0(new Vector3f(x, y0, 0.0f));
			ellipses[i].setPosition1(new Vector3f(x, y1, 0.0f));
		}
		
		// your update logic here
		
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
		
		// gray background
		target.setClearColor(new Color(0.1f));
		
		// create canvas
		canvas = new Canvas();
		canvas.setViewport(1280.0f, 720.0f);
		
		// create ellipses
		for(int i = 0; i < 32; i++) {
			CanvasEllipse ellipse = new CanvasEllipse(canvas);
			ellipse.setStrokeStyle(new StrokeStyle(4.0f, Color.white));
			ellipses.Add(ellipse);
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
		
		// create canvas
		if(!canvas.create(device, target)) return false;
		
		// window target
		target.begin();
		{
			Command command = device.createCommand(target);
			
			// draw canvas
			canvas.draw(command, target);
			
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
	
	private App app;							// Application interface
	private Window window = Window.Null();		// Window interface
	private Device device = Device.Null();		// Device interface
	private Target target = Target.Null();		// Target interface
	private Canvas canvas = Canvas.Null();		// Canvas interface
	
	private List<CanvasEllipse> ellipses;
}

/*
 */
class AppCanvas {
	
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
