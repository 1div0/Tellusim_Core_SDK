// Tellusim Fragment Application Template

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
		
		// create resources
		if !create_resources() { return false }
		
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
		
		// fragment uniform parameters
		struct FragmentParameters {
			var aspect: Float32
			var time: Float32
		}
		var parameters = FragmentParameters (
			aspect: Float32(window.width()) / Float32(window.height()),
			time: Float32(Time.seconds())
		)
		
		// draw fullscreen triangle
		command.setPipeline(pipeline)
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
	
	// helper function to create and load resources
	private func create_resources() -> Bool {
		
		// create pipeline
		pipeline = device.createPipeline()
		pipeline.setUniformMask(0, Shader.Mask.Fragment)
		pipeline.setColorFormat(window.colorFormat())
		pipeline.setDepthFormat(window.depthFormat())
		pipeline.setDepthFunc(Pipeline.DepthFunc.Always)
		if !pipeline.loadShaderGLSL(Shader.Kind.Vertex, "main.shader", "VERTEX_SHADER=1") {
			Log.print(Log.Level.Error, "Application::create_resources(): can't load Vertex shader\n")
			return false
		}
		if !pipeline.loadShaderGLSL(Shader.Kind.Fragment, "main.shader", "FRAGMENT_SHADER=1") {
			Log.print(Log.Level.Error, "Application::create_resources(): can't load Fragment shader\n")
			return false
		}
		if !pipeline.create() {
			Log.print(Log.Level.Error, "Application::create_resources(): can't create Pipeline\n")
			return false
		}
		
		// your resource creation code here
		
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
	
	private var pipeline: Pipeline!		// Pipeline interface
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
