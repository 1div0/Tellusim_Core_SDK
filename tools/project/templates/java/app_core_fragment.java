// Tellusim Fragment Application Template

package com.main;

import com.tellusim.*;
import java.nio.ByteOrder;
import java.nio.ByteBuffer;

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
		
		// your update logic here
		
		return true;
	}
	
	// render function to handle the drawing commands
	public boolean render(Command command) {
		
		// compute uniform parameters
		ByteBuffer parameters = ByteBuffer.allocate(8).order(ByteOrder.LITTLE_ENDIAN);
		parameters.putFloat((float)window.getWidth() / (float)window.getHeight());
		parameters.putFloat((float)Time.seconds());
		
		// draw fullscreen triangle
		command.setPipeline(pipeline);
		command.setUniform(0, parameters);
		command.drawArrays(3);
		
		// your rendering code here
		
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
	private boolean render_window() {
		
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
		System.gc();
		
		return true;
	}
	
	private App app;				// Application interface
	private Window window;			// Window interface
	private Device device;			// Device interface
	private Target target;			// Target interface
	
	private Pipeline pipeline;		// Pipeline interface
}
