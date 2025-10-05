// Tellusim Canvas Application Template

package com.main;

import com.tellusim.*;
import java.util.ArrayList;

/*
 */
public class Application {
	
	// constructor initializing the application with command-line arguments
	public Application(String[] args) {
		app = new App(args);
	}
	
	// create function responsible for initialization
	public boolean create() {
		
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
	public boolean update() {
		
		// update canvas elements
		float hheight = canvas.getHeight() * 0.5f;
		for(int i = 0; i < ellipses.size(); i++) {
			float angle = Base.Pi2 * i / ellipses.size() + (float)Time.seconds();
			float x = canvas.getWidth() * (1 + i) / (ellipses.size() + 1);
			float y0 = hheight + (float)(Math.cos(angle * 2.0f - 0.2f) * Math.sin(angle - 0.1f)) * hheight * 0.9f;
			float y1 = hheight + (float)(Math.cos(angle * 2.0f + 0.2f) * Math.sin(angle + 0.1f)) * hheight * 0.9f;
			ellipses.get(i).setColor(Color.hsv(angle * Base.Rad2Deg + y0 * 0.5f, 1.0f, 1.0f));
			ellipses.get(i).setRadius((float)Math.sqrt(20.0f * 20.0f + (y1 - y0) * (y1 - y0) * 0.25f));
			ellipses.get(i).setPosition0(new Vector3f(x, y0, 0.0f));
			ellipses.get(i).setPosition1(new Vector3f(x, y1, 0.0f));
		}
		
		// your update logic here
		
		return true;
	}
	
	// main run loop
	public boolean run() {
		
		// run application
		window.run(new Window.MainLoopCallback() {
			public boolean run() {
				return render_window();
			}
		});
		
		// finish context
		window.finish();
		
		// keep window alive
		window.unacquirePtr();
		
		return true;
	}
	
	// helper function to create and initialize the Window
	private boolean create_window() {
		
		// create Window
		window = new Window(app.getPlatform(), app.getDevice());
		if(!window.isValidPtr() || !window.setSize(app.getWidth(), app.getHeight()) ||
			!window.create(window.getPlatformName() + " Java @NAME@") || !window.setHidden(false)) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Window\n");
			return false;
		}
		
		// set Window callbacks
		window.setKeyboardPressedCallback(new Window.KeyboardPressedCallback() {
			public void run(int key, int code) {
				if(key == Window.Key.F4.value && window.getKeyboardKey(Window.Key.Alt.value)) window.stop();
				if(key == Window.Key.Esc.value) window.stop();
			}
		});
		window.setCloseClickedCallback(new Window.CloseClickedCallback() {
			public void run() { window.stop(); }
		});
		
		// create Device
		device = new Device(window);
		if(!device.isValidPtr()) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Device\n");
			return false;
		}
		
		// create Target
		target = device.createTarget(window);
		if(!target.isValidPtr()) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Target\n");
			return false;
		}
		
		return true;
	}
	
	// helper function to create and load resources
	private boolean create_resources() {
		
		// gray background
		target.setClearColor(new Color(0.1f));
		
		// create canvas
		canvas = new Canvas();
		canvas.setViewport(1280.0f, 720.0f);
		
		// create ellipses
		ellipses = new ArrayList<>();
		for(int i = 0; i < 32; i++) {
			CanvasEllipse ellipse = new CanvasEllipse(canvas);
			ellipse.setStrokeStyle(new StrokeStyle(4.0f, Color.white()));
			ellipses.add(ellipse);
		}
		
		// your resource creation code here
		
		return true;
	}
	
	// helper function to render the Window
	private boolean render_window() {
		
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
		System.gc();
		
		return true;
	}
	
	private App app;				// Application interface
	private Window window;			// Window interface
	private Device device;			// Device interface
	private Target target;			// Target interface
	private Canvas canvas;			// Canvas interface
	
	private ArrayList<CanvasEllipse> ellipses;
}
