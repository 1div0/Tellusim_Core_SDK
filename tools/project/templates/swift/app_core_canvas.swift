// Tellusim Canvas Application Template

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
		
		// update canvas elements
		let hheight = canvas.height() * 0.5
		for i in 0..<ellipses.count {
			let angle = Pi2 * Float32(i) / Float32(ellipses.count) + Float32(Time.seconds())
			let x = canvas.width() * Float32(1 + i) / Float32(ellipses.count + 1)
			let y0 = hheight + cos(angle * 2.0 - 0.2) * sin(angle - 0.1) * hheight * 0.9
			let y1 = hheight + cos(angle * 2.0 + 0.2) * sin(angle + 0.1) * hheight * 0.9
			ellipses[i].setColor(Color.hsv(angle * Rad2Deg + y0 * 0.5, 1.0, 1.0))
			ellipses[i].setRadius(sqrt(20.0 * 20.0 + (y1 - y0) * (y1 - y0) * 0.25))
			ellipses[i].setPosition0(Vector3f(x, y0, 0.0))
			ellipses[i].setPosition1(Vector3f(x, y1, 0.0))
		}
		
		// your update logic here
		
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
		
		// gray background
		target.setClearColor(Color(0.1))
		
		// create canvas
		canvas = Canvas()
		canvas.setViewport(1280.0, 720.0)
		
		// create ellipses
		for _ in 0..<32 {
			let ellipse = CanvasEllipse(canvas)
			ellipse.setStrokeStyle(StrokeStyle(4.0, Color.white))
			ellipses.append(ellipse)
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
		
		// create canvas
		if !canvas.create(device, target) { return false }
		
		// window target
		if target.begin() {
			
			let command = device.createCommand(target)
			
			// draw canvas
			canvas.draw(command, target)
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
	private var canvas: Canvas!			// Canvas interface
	
	private var ellipses: [CanvasEllipse] = []
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
