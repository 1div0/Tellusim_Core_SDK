// Tellusim Engine SDK Application Template

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
		
		// create scene
		if !create_scene() { return false }
		
		// create render
		if !create_render() { return false }
		
		// load scene
		let stream = scene_manager.stream()
		stream.loadGraph(scene, "scene.graphx", SceneStream.GraphFunction({ [self] (_ stream_id: UInt32, graph: Graph) in
			Log.print(Log.Level.Message, "Application::create(): scene is loaded\n")
			torus_node = NodeObject(base: graph.node("Torus Node"))
		}), SceneStream.Flags.Default)
		
		// additional initialization code can go here
		
		return true
	}
	
	// update function for logic updates
	func update() -> Bool {
		
		// update scene
		let angle = Time.seconds() * 16.0
		node_camera.setGlobalTransform(Matrix4x3d.placeTo(Matrix4x3d.rotateZ(-angle) * Vector3d(2.0, 2.0, 1.0), Vector3d.zero, Vector3d(0.0, 0.0, 1.0)))
		node_camera.updateScene()
		if torus_node != nil {
			torus_node.setPivotTransform(Matrix4x3f.rotateX(Float32(angle) * 3.0) * Matrix4x3f.rotateZ(Float32(angle) * 2.0))
			torus_node.updateScene()
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
		
		// finish device
		device.finish()
		
		// stop process thread
		scene_manager.terminate()
		
		// release render
		render_frame.clearPtr()
		render_manager.clearPtr()
		
		// clear scene
		scene.clear()
		scene_manager.update(device, main_async)
		device.finish()
		
		// release scene
		scene.clearPtr()
		scene_manager.clearPtr()
		
		// done
		Log.print(Log.Level.Message, "Done\n")
		
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
	
	// helper function to create and initialize Scene
	private func create_scene(_ flags: SceneManager.Flags = SceneManager.Flags.Default) -> Bool {
		
		// main async
		main_async = Async()
		if !main_async.initialize() {
			Log.print(Log.Level.Error, "Application::create_scene(): can't initialize main Async\n")
			return false
		}
		
		// process async
		process_async = Async()
		if !process_async.initialize() {
			Log.print(Log.Level.Error, "Application::create_scene(): can't initialize process Async\n")
			return false
		}
		
		// cache location
		SceneManager.setShaderCache("shader.cache")
		SceneManager.setTextureCache("texture.cache")
		
		Log.print(Log.Level.Verbose, "Application::create_scene(): create SceneManager\n")
		
		// create scene manager
		scene_manager = SceneManager()
		if !scene_manager.create(device, flags, nil, &main_async) {
			Log.print(Log.Level.Error, "Application::create_scene(): can't create SceneManager\n")
			return false
		}
		
		Log.print(Log.Level.Verbose, "Application::create_scene(): SceneManager is ready\n")
		
		// create process thread
		class ProcessThread : Thread {
			init(_ manager: SceneManager, _ async : Async) {
				self.manager = manager
				self.async = async
				super.init()
			}
			override func main() {
				while manager.isValidPtr() && !manager.isTerminated() {
					if !manager.process(async) { Time.sleep(1000) }
				}	
				Log.print(Log.Level.Message, "Thread Done\n")
			}
			var manager: SceneManager
			var async: Async
		}
		process_thread = ProcessThread(scene_manager, process_async)
		process_thread.start()
		
		// initialize scene
		scene = Scene(scene_manager)
		graph = Graph(scene)
		
		// create camera
		camera = CameraPerspective(scene)
		node_camera = NodeCamera(graph, camera)
		
		return true
	}
	
	// helper function to create and initialize Render
	private func create_render(_ manager_flags: RenderManager.Flags = RenderManager.Flags.ShadowMap | RenderManager.Flags.Antialiasing | RenderManager.Flags.MotionBuffer, _ renderer_flags: RenderRenderer.Flags = RenderRenderer.Flags.MotionBuffer | RenderRenderer.Flags.ScreenOcclusion | RenderRenderer.Flags.ScreenReflection | RenderRenderer.Flags.Antialiasing | RenderRenderer.Flags.MotionBuffer) -> Bool {
		
		Log.print(Log.Level.Verbose, "Application::create_render(): create RenderManager\n")
		
		// create render manager
		render_manager = RenderManager(scene_manager)
		render_manager.setRendererFlags(device, renderer_flags)
		render_manager.setDrawParameters(device, window.colorFormat(), window.depthFormat(), window.multisample())
		if !render_manager.create(device, manager_flags, nil, &main_async) {
			Log.print(Log.Level.Error, "Application::create_render(): can't create manager\n")
			return false
		}
		
		Log.print(Log.Level.Verbose, "Application::create_render(): RenderManager is ready\n")
		
		render_renderer = render_manager.renderer()
		render_spatial = render_manager.spatial()
		
		// create render frame
		render_frame = RenderFrame(render_manager, node_camera)
		
		// scene render
		scene.setRender(render_renderer.sceneRender())
		
		return true
	}
	
	// helper function to render the Scene
	private func render_scene() -> Bool {
		
		// update RenderManager
		render_manager.update()
		
		// render flags
		let spatial_flags = RenderSpatial.Flags.Antialiasing
		let renderer_flags = render_manager.rendererFlags()
		
		// dispatch compute
		do {
			
			let compute = device.createCompute()
			
			// dispatch frame intersection
			render_spatial.dispatchFrame(compute, render_frame, spatial_flags)
			
			// dispatch shadow map intersection
			render_spatial.dispatchShadowMap(compute, render_frame, spatial_flags)
			
			// dispatch shadow map command generation
			render_renderer.dispatchShadowMap(compute, render_frame, renderer_flags)
		}
		
		// render shadow map
		do {
			
			// flush buffers
			scene_manager.flush(device)
			render_manager.flush(device)
			render_frame.flush(device)
			
			// draw deferred
			render_renderer.drawShadowMap(device, render_frame, renderer_flags)
		}
		
		// dispatch compute
		do {
			
			let compute = device.createCompute()
			
			// dispatch frame object intersection
			render_spatial.dispatchObjects(compute, render_frame, spatial_flags)
			
			// dispatch draw command generation
			render_renderer.dispatchFrame(compute, render_frame, renderer_flags)
		}
		
		// render frame
		do {
			
			// flush buffers
			scene_manager.flush(device)
			render_manager.flush(device)
			render_frame.flush(device)
			
			// draw deferred
			render_renderer.drawDeferred(device, render_frame, renderer_flags)
		}
		
		// dispatch compute
		do {
			
			let compute = device.createCompute()
			
			// dispatch render
			render_renderer.dispatchScreen(device, compute, render_frame, renderer_flags)
			render_renderer.dispatchOcclusion(device, compute, render_frame, renderer_flags)
			render_renderer.dispatchLight(device, compute, render_frame, renderer_flags)
			render_renderer.dispatchOccluder(device, compute, render_frame, renderer_flags)
			render_renderer.dispatchFeedback(device, compute, render_frame, renderer_flags)
			render_renderer.dispatchLuminance(device, compute, render_frame, renderer_flags)
			render_renderer.dispatchComposite(device, compute, render_frame, renderer_flags)
		}
		
		return true
	}
	
	// helper function to render the Window
	private func render_window(_ scene_flags: SceneManager.Flags = SceneManager.Flags.None, _ usec: UInt64 = 10000) -> Bool {
		
		// update events
		Window.update()
		
		// update application
		if !update() { return false }
		
		// render window
		if !window.render() { return false }
		
		// resize RenderFrame with 200% upscale
		let width = window.width()
		let height = window.height()
		let texture = render_frame.compositeTexture()
		if !texture.isValidPtr() || texture.width() != width || texture.height() != height {
			if !device.finish() { return false }
			render_frame.create(device, render_renderer, width / 2, height / 2, TSSize(width, height))
		}
		
		// create Scene
		if !scene.create(device, &main_async) {
			Log.print(Log.Level.Error, "Application::render_window(): can't create Scene\n")
			return false
		}
		
		// update Scene
		scene.setTime(Time.seconds())
		scene.update(device)
		
		// update SceneManager
		if !scene_manager.update(device, main_async, counter, 0, usec, scene_flags) {
			Log.print(Log.Level.Error, "Application::render_window(): can't update SceneManager\n")
			return false
		}
		counter += 1
		
		// dispatch compute
		do {
			
			let compute = device.createCompute()
			
			// dispatch SceneManager
			scene_manager.dispatch(device, compute)
			
			// dispatch Scene
			scene.dispatch(device, compute)
			
			compute.destroyPtr()
		}
		
		// render scene
		if !render_scene() {
			Log.print(Log.Level.Error, "Application::render_window(): can't render RenderFrame\n")
			return false
		}
		
		// window target
		if target.begin() {
			let command = device.createCommand(target)
			let texture = render_frame.compositeTexture()
			let flipped = (target.isFlipped() != render_renderer.isTargetFlipped())
			render_manager.draw().drawTexture(command, texture, flipped, RenderDraw.Mode.Composite)
			target.end()
		}
		
		// present window
		if !window.present() { return false }
		
		// check errors
		if !device.check() { return false }
		
		return true
	}
	
	private var app: App							// Application interface
	private var window: Window!						// Window interface
	private var device: Device!						// Device interface
	private var target: Target!						// Target interface
	
	private var main_async: Async!					// main Async
	private var process_async: Async!				// process Async
	private var process_thread: Thread!				// process Thread
	
	private var scene_manager: SceneManager!		// SceneManager interface
	private var scene: Scene!						// Scene interface
	
	private var graph: Graph!						// Graph interface
	private var node_camera: NodeCamera!			// NodeCamera interface
	private var camera: CameraPerspective!			// CameraPerspective interface
	
	private var torus_node: NodeObject!				// NodeObject interface
	
	private var render_manager: RenderManager!		// RenderManager interface
	private var render_renderer: RenderRenderer!	// RenderRenderer interface
	private var render_spatial: RenderSpatial!		// RenderSpatial interface
	private var render_frame: RenderFrame!			// RenderFrame interface
	
	private var counter: UInt64 = 0					// frame counter
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
