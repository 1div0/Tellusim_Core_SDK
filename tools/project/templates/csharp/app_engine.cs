// Tellusim Engine SDK Application Template

#define LOAD_SCENE

using System;
using Tellusim;
using System.Threading;

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
		
		// create scene
		if(!create_scene()) return false;
		
		// create render
		if(!create_render()) return false;
		
		// load scene
		#if LOAD_SCENE
			SceneStream stream = scene_manager.getStream();
			stream.loadGraph(scene, "scene.graphx", (uint stream_id, Graph graph, IntPtr data) => {
				Log.print(Log.Level.Message, "Application::create(): scene is loaded\n");
				torus_node = new NodeObject(graph.getNode("Torus Node"));
			}, SceneStream.Flags.Default);
		#endif
		
		// additional initialization code can go here
		
		return true;
	}
	
	// update function for logic updates
	public bool update() {
		
		// update scene
		#if LOAD_SCENE
			float angle = (float)Time.seconds() * 16.0f;
			node_camera.setGlobalTransform(Matrix4x3d.placeTo(Matrix4x3d.rotateZ(-angle) * new Vector3d(2.0, 2.0, 1.0), Vector3d.zero, new Vector3d(0.0, 0.0, 1.0)));
			node_camera.updateScene();
			if(torus_node) {
				torus_node.setPivotTransform(Matrix4x3f.rotateX(angle * 3.0f) * Matrix4x3f.rotateZ(angle * 2.0f));
				torus_node.updateScene();
			}
		#endif
		
		// your update logic here
		
		return true;
	}
	
	// main run loop
	public bool run() {
		
		// run application
		window.run((IntPtr data) => {
			return render_window();
		});
		
		// stop process thread
		scene_manager.terminate();
		
		// finish device
		device.finish();
		
		// release render
		render_frame.clearPtr();
		render_manager.clearPtr();
		
		// clear scene
		scene.clear();
		scene_manager.update(device, main_async);
		device.finish();
		
		// wait thread
		process_thread.Join();
		
		// release scene
		scene.clearPtr();
		scene_manager.clearPtr();
		
		// keep window alive
		window.unacquirePtr();
		
		Log.print(Log.Level.Message, "Done\n");
		
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
	
	// helper function to create and initialize Scene
	bool create_scene(SceneManager.Flags manager_flags = SceneManager.Flags.Default) {
		
		// main async
		main_async = new Async();
		if(!main_async.init()) {
			Log.print(Log.Level.Error, "Application::create_scene(): can't initialize main Async\n");
			return false;
		}
		
		// process async
		process_async = new Async();
		if(!process_async.init()) {
			Log.print(Log.Level.Error, "Application::create_scene(): can't initialize process Async\n");
			return false;
		}
		
		// cache location
		SceneManager.setShaderCache("shader.cache");
		SceneManager.setTextureCache("texture.cache");
		
		Log.print(Log.Level.Verbose, "Application::create_scene(): create SceneManager\n");
		
		// create scene manager
		scene_manager = new SceneManager();
		if(!scene_manager.create(device, manager_flags, null, main_async)) {
			Log.print(Log.Level.Error, "Application::create_scene(): can't create SceneManager\n");
			return false;
		}
		
		Log.print(Log.Level.Verbose, "Application::create_scene(): SceneManager is ready\n");
		
		// create process thread
		process_thread = new Thread(() => {
			while(scene_manager && !scene_manager.isTerminated()) {
				if(!scene_manager.process(process_async)) Time.sleep(1000);
			}
			Log.print(Log.Level.Message, "Thread Done\n");
		});
		process_thread.Start();
		
		// initialize scene
		scene = new Scene(scene_manager);
		graph = new Graph(scene);
		
		// create camera
		camera = new CameraPerspective(scene);
		node_camera = new NodeCamera(graph, camera);
		
		return true;
	}
	
	// helper function to create and initialize Render
	bool create_render(RenderManager.Flags manager_flags = RenderManager.Flags.ShadowMap | RenderManager.Flags.Antialiasing | RenderManager.Flags.MotionBuffer, RenderRenderer.Flags renderer_flags = RenderRenderer.Flags.MotionBuffer | RenderRenderer.Flags.ScreenOcclusion | RenderRenderer.Flags.ScreenReflection | RenderRenderer.Flags.Antialiasing | RenderRenderer.Flags.MotionBuffer) {
		
		Log.print(Log.Level.Verbose, "Application::create_render(): create RenderManager\n");
		
		// create render manager
		render_manager = new RenderManager(scene_manager);
		render_manager.setRendererFlags(device, renderer_flags);
		render_manager.setDrawParameters(device, window.getColorFormat(), window.getDepthFormat(), window.getMultisample());
		if(!render_manager.create(device, manager_flags, null, main_async)) {
			Log.print(Log.Level.Error, "Application::create_render(): can't create manager\n");
			return false;
		}
		
		Log.print(Log.Level.Verbose, "Application::create_render(): RenderManager is ready\n");
		
		render_renderer = render_manager.getRenderer();
		render_spatial = render_manager.getSpatial();
		
		// create render frame
		render_frame = new RenderFrame(render_manager, node_camera);
		
		// scene render
		scene.setRender(render_renderer.getSceneRender());
		
		return true;
	}
	
	// helper function to render the Scene
	private bool render_scene() {
		
		// update RenderManager
		render_manager.update();
		
		// render flags
		RenderSpatial.Flags spatial_flags = RenderSpatial.Flags.Antialiasing;
		RenderRenderer.Flags renderer_flags = render_manager.getRendererFlags();
		
		// dispatch compute
		{
			Compute compute = device.createCompute();
			
			// dispatch frame intersection
			render_spatial.dispatchFrame(compute, render_frame, spatial_flags);
			
			// dispatch shadow map intersection
			render_spatial.dispatchShadowMap(compute, render_frame, spatial_flags);
			
			// dispatch shadow map command generation
			render_renderer.dispatchShadowMap(compute, render_frame, renderer_flags);
			
			compute.destroyPtr();
		}
		
		// render shadow map
		{
			// flush buffers
			scene_manager.flush(device);
			render_manager.flush(device);
			render_frame.flush(device);
			
			// draw deferred
			render_renderer.drawShadowMap(device, render_frame, renderer_flags);
		}
		
		// dispatch compute
		{
			Compute compute = device.createCompute();
			
			// dispatch frame object intersection
			render_spatial.dispatchObjects(compute, render_frame, spatial_flags);
			
			// dispatch draw command generation
			render_renderer.dispatchFrame(compute, render_frame, renderer_flags);
			
			compute.destroyPtr();
		}
		
		// render frame
		{
			// flush buffers
			scene_manager.flush(device);
			render_manager.flush(device);
			render_frame.flush(device);
			
			// draw deferred
			render_renderer.drawDeferred(device, render_frame, renderer_flags);
		}
		
		// dispatch compute
		{
			Compute compute = device.createCompute();
			
			// dispatch render
			render_renderer.dispatchScreen(device, compute, render_frame, renderer_flags);
			render_renderer.dispatchOcclusion(device, compute, render_frame, renderer_flags);
			render_renderer.dispatchLight(device, compute, render_frame, renderer_flags);
			render_renderer.dispatchOccluder(device, compute, render_frame, renderer_flags);
			render_renderer.dispatchFeedback(device, compute, render_frame, renderer_flags);
			render_renderer.dispatchLuminance(device, compute, render_frame, renderer_flags);
			render_renderer.dispatchComposite(device, compute, render_frame, renderer_flags);
			
			compute.destroyPtr();
		}
		
		return true;
	}
	
	// helper function to render the Window
	private bool render_window(SceneManager.Flags scene_flags = SceneManager.Flags.None, ulong usec = 10000) {
		
		// update events
		Window.update();
		
		// update application
		if(!update()) return false;
		
		// render window
		if(!window.render()) return false;
		
		// resize RenderFrame with 200% upscale
		uint width = window.getWidth();
		uint height = window.getHeight();
		Texture texture = render_frame.getCompositeTexture();
		if(!texture || texture.getWidth() != width || texture.getHeight() != height) {
			if(!device.finish()) return false;
			render_frame.create(device, render_renderer, width / 2, height / 2, new Size(width, height));
		}
		
		// create Scene
		if(!scene.create(device, main_async)) {
			Log.print(Log.Level.Error, "Application::render_window(): can't create Scene\n");
			return false;
		}
		
		// update Scene
		scene.setTime(Time.seconds());
		scene.update(device);
		
		// update SceneManager
		if(!scene_manager.update(device, main_async, counter++, 0, usec, scene_flags)) {
			Log.print(Log.Level.Error, "Application::render_window(): can't update SceneManager\n");
			return false;
		}
		
		// dispatch compute
		{
			Compute compute = device.createCompute();
			
			// dispatch SceneManager
			scene_manager.dispatch(device, compute);
			
			// dispatch Scene
			scene.dispatch(device, compute);
			
			compute.destroyPtr();
		}
		
		// render scene
		if(!render_scene()) {
			Log.print(Log.Level.Error, "Application::render_window(): can't render RenderFrame\n");
			return false;
		}
		
		// window target
		target.begin();
		{
			Command command = device.createCommand(target);
			texture = render_frame.getCompositeTexture();
			bool flipped = (target.isFlipped() ^ render_renderer.isTargetFlipped());
			render_manager.getDraw().drawTexture(command, texture, flipped, RenderDraw.Mode.Composite);
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
	
	private App app;												// Application interface
	private Window window = Window.Null();							// Window interface
	private Device device = Device.Null();							// Device interface
	private Target target = Target.Null();							// Target interface
	
	private Async main_async = Async.Null();						// main Async
	private Async process_async = Async.Null();						// process Async
	private Thread process_thread;									// process Thread
	
	private SceneManager scene_manager = SceneManager.Null();		// SceneManager interface
	private Scene scene = Scene.Null();								// Scene interface
	
	private Graph graph = Graph.Null();								// Graph interface
	private NodeCamera node_camera = NodeCamera.Null();				// NodeCamera interface
	private CameraPerspective camera = CameraPerspective.Null();	// CameraPerspective interface
	
	#if LOAD_SCENE
		private NodeObject torus_node = NodeObject.Null();			// NodeObject interface
	#endif
	
	private RenderManager render_manager = RenderManager.Null();	// RenderManager interface
	private RenderRenderer render_renderer = RenderRenderer.Null();	// RenderRenderer interface
	private RenderSpatial render_spatial = RenderSpatial.Null();	// RenderSpatial interface
	private RenderFrame render_frame = RenderFrame.Null();			// RenderFrame interface
	
	private ulong counter = 0;										// frame counter
}

/*
 */
class AppEngine {
	
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
