// Tellusim Core SDK Application Template

package com.main;

import com.tellusim.*;

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
		
		// create pipeline
		if(!create_pipeline()) return false;
		
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
		
		// render pipeline
		command.setPipeline(pipeline);
		float angle = (float)Time.seconds() * 64.0f;
		float iaspect = (float)window.getHeight() / window.getWidth();
		command.setUniform(0, Matrix4x4f.rotateZ(angle).mul(Matrix4x4f.scale(iaspect, 1.0f, 1.0f)));
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
	
	// helper function to create the Pipeline
	private boolean create_pipeline() {
		
		// create and configure the rendering pipeline
		pipeline = device.createPipeline();
		pipeline.setUniformMask(0, Shader.Mask.Vertex);
		pipeline.setColorFormat(window.getColorFormat());
		pipeline.setDepthFormat(window.getDepthFormat());
		pipeline.setDepthFunc(Pipeline.DepthFunc.Always);
		
		// vertex Shader that defines a triangle and transforms its position
		if(!pipeline.createShaderGLSL(Shader.Type.Vertex, """
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
		""")) return false;
		
		// fragment shader
		if(!pipeline.createShaderGLSL(Shader.Type.Fragment, """
			layout(location = 0) in vec4 s_color;
			layout(location = 0) out vec4 out_color;
			void main() {
				out_color = s_color;
			}
		""")) return false;
		
		if(!pipeline.create()) return false;
		
		target.setClearColor(new Color(0.2f));
		
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
	
	private Pipeline pipeline;		// Pipeline for triangle rendering
}
