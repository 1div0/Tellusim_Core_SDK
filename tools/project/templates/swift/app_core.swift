// Tellusim Core SDK Application Template

import Tellusim

/*
 */
class Application {
	
	// constructor initializing the application
	init() {
		app = App()
	}
	
	// create function responsible for initialization
	func create() -> Bool {
		
		// create app
		if !app.create() {
			Log.print(Log.Level.Error, "Application::create(): can't create App\n")
			return false
		}
		
		// create window
		if !create_window() { return false }
		
		// create pipeline
		if !create_pipeline() { return false }
		
		// additional initialization code can go here
		
		return true
	}
	
	// update function for logic updates
	func update() -> Bool {
		
		// your update logic here
		
		return true
	}
	
	// render function to handle the drawing commands
	func render(_ command: Command) -> Bool {
		
		// render pipeline
		command.setPipeline(pipeline)
		let angle = Float32(Time.seconds() * 64.0)
		let iaspect = Float32(window.height()) / Float32(window.width())
		var parameters = Matrix4x4f.rotateZ(angle) * Matrix4x4f.scale(iaspect, 1.0, 1.0)
		command.setUniform(0, &parameters)
		command.drawArrays(3)
		
		// your rendering code here
		
		return true
	}
	
	// main run loop
	func run() -> Bool {
		
		// run application
		window.run(Window.MainLoopFunction({ [self] () -> Bool in
			return render_window()
		}))
		
		// finish context
		window.finish()
		
		return true
	}
	
	// helper function to create and initialize the Window
	private func create_window() -> Bool {
		
		// create Window
		window = Window(app.platform(), app.device())
		if !window || !window.setSize(app.width(), app.height()) ||
			!window.create(window.platformName() + " Swift @NAME@") || !window.setHidden(false) {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Window\n")
			return false
		}
		
		// set Window callbacks
		window.setKeyboardPressedCallback(Window.KeyboardPressedFunction({ [self] (key: UInt32, code: UInt32) in
			if key == Window.Key.F4.rawValue && window.keyboardKey(UInt32(Window.Key.Alt.rawValue)) { window.stop() }
			if key == Window.Key.Esc.rawValue { window.stop() }
		}))
		window.setCloseClickedCallback(Window.CloseClickedFunction({ [self] () in window.stop() }))
		
		// create Device
		device = Device(window)
		if !device {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Device\n")
			return false
		}
		
		// create Target
		target = device.createTarget(window)
		if !target  {
			Log.print(Log.Level.Error, "Application::create_window(): can't create Target\n")
			return false
		}
		
		return true
	}
	
	// helper function to create the Pipeline
	private func create_pipeline() -> Bool {
		
		// create and configure the rendering pipeline
		pipeline = device.createPipeline()
		pipeline.setUniformMask(0, Shader.Mask.Vertex)
		pipeline.setColorFormat(window.colorFormat())
		pipeline.setDepthFormat(window.depthFormat())
		pipeline.setDepthFunc(Pipeline.DepthFunc.Always)
		
		// vertex Shader that defines a triangle and transforms its position
		if !pipeline.createShaderGLSL(Shader.Kind.Vertex, """
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
		""") { return false }
		
		// fragment shader
		if !pipeline.createShaderGLSL(Shader.Kind.Fragment, """
			layout(location = 0) in vec4 s_color;
			layout(location = 0) out vec4 out_color;
			void main() {
				out_color = s_color;
			}
		""") { return false }
		
		if !pipeline.create() { return false }
		
		target.setClearColor(Color(0.2))
		
		return true
	}
	
	// helper function to render the Window
	private func render_window() -> Bool {
		
		// update events
		Window.update()
		
		// update application
		if !update() { return false }
		
		// render window
		if !window.render() { return false }
		
		// window target
		if target.begin() {
			
			let command = device.createCommand(target)
			
			// render application
			if !render(command) {
				target.end()
				return false
			}
		}
		target.end()
		
		// present window
		if !window.present() { return false }
		
		// check errors
		if !device.check() { return false }
		
		return true
	}
	
	private var app: App				// Application interface
	private var window: Window!			// Window interface
	private var device: Device!			// Device interface
	private var target: Target!			// Target interface
	
	private var pipeline: Pipeline!		// Pipeline for triangle rendering
}

/*
 */
func main_() -> Int32 {
	
	// instantiate the application
	let application = Application()
	
	// create the application
	if !application.create() { return 1 }
	
	// run the application
	if !application.run() { return 1 }
	
	return 0
}

/*
 */
#if os(macOS)
	exit(main_())
#elseif os(iOS)
	import UIKit
	UIApplicationMain(CommandLine.argc, CommandLine.unsafeArgv, nil, NSStringFromClass(AppDelegate.self))
#endif
