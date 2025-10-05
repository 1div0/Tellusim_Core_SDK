// Tellusim Engine SDK Application Template

package com.main;

import com.tellusim.*;

/*
 */
class Application {
	
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
		
		// create scene
		if(!create_scene()) return false;
		
		// create render
		if(!create_render()) return false;
		
		// load scene
		SceneStream stream = scene_manager.getStream();
		stream.loadGraph(scene, "scene.graphx", new SceneStream.GraphCallback() {
			public void run(int stream_id, Graph graph) {
				Log.print(Log.Level.Message, "Application::create(): scene is loaded\n");
				torus_node = new NodeObject(graph.getNode("Torus Node"));
			}
		}, SceneStream.Flags.Default);
		
		// additional initialization code can go here
		
		return true;
	}
	
	// update function for logic updates
	public boolean update() {
		
		// update scene
		float angle = (float)Time.seconds() * 16.0f;
		node_camera.setGlobalTransform(Matrix4x3d.placeTo(Matrix4x3d.rotateZ(-angle).mul(new Vector3d(2.0, 2.0, 1.0)), Vector3d.zero(), new Vector3d(0.0, 0.0, 1.0)));
		node_camera.updateScene();
		if(torus_node != null && torus_node.isValidPtr()) {
			torus_node.setPivotTransform(Matrix4x3f.rotateX(angle * 3.0f).mul(Matrix4x3f.rotateZ(angle * 2.0f)));
			torus_node.updateScene();
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
		try {
			process_thread.join();
		} catch(Exception e) {
			System.out.println(e);
		}
		
		// release scene
		scene.clearPtr();
		scene_manager.clearPtr();
		
		// keep window alive
		window.unacquirePtr();
		
		Log.print(Log.Level.Message, "Done\n");
		
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
	
	// helper function to create and initialize Scene
	boolean create_scene() {
		
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
		
		// scene flags
		SceneManager.Flags.Enum manager_flags = SceneManager.Flags.Default;
		
		// create scene manager
		scene_manager = new SceneManager();
		if(!scene_manager.create(device, manager_flags, null, main_async)) {
			Log.print(Log.Level.Error, "Application::create_scene(): can't create SceneManager\n");
			return false;
		}
		
		Log.print(Log.Level.Verbose, "Application::create_scene(): SceneManager is ready\n");
		
		// create process thread
		process_thread = new Thread() {
			public void run() {
				while(scene_manager.isValidPtr() && !scene_manager.isTerminated()) {
					if(!scene_manager.process(process_async)) Time.sleep(1000);
				}
				Log.print(Log.Level.Message, "Thread Done\n");
			}
		};
		process_thread.start();
		
		// initialize scene
		scene = new Scene(scene_manager);
		graph = new Graph(scene);
		
		// create camera
		camera = new CameraPerspective(scene);
		node_camera = new NodeCamera(graph, camera);
		
		return true;
	}
	
	// helper function to create and initialize Render
	boolean create_render() {
		
		Log.print(Log.Level.Verbose, "Application::create_render(): create RenderManager\n");
		
		// render flags
		RenderManager.Flags manager_flags = RenderManager.Flags.ShadowMap.or(RenderManager.Flags.Antialiasing.or(RenderManager.Flags.MotionBuffer));
		RenderRenderer.Flags renderer_flags = RenderRenderer.Flags.MotionBuffer.or(RenderRenderer.Flags.ScreenOcclusion.or(RenderRenderer.Flags.ScreenReflection.or(RenderRenderer.Flags.Antialiasing.or(RenderRenderer.Flags.MotionBuffer))));
		
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
	private boolean render_scene() {
		
		// update RenderManager
		render_manager.update();
		
		// render flags
		RenderSpatial.Flags.Enum spatial_flags = RenderSpatial.Flags.Antialiasing;
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
	private boolean render_window() {
		
		// update events
		Window.update();
		
		// update application
		if(!update()) return false;
		
		// render window
		if(!window.render()) return false;
		
		// resize RenderFrame with 200% upscale
		int width = window.getWidth();
		int height = window.getHeight();
		Texture texture = render_frame.getCompositeTexture();
		if(!texture.isValidPtr() || texture.getWidth() != width || texture.getHeight() != height) {
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
		
		// scene flags
		long usec = 10000;
		SceneManager.Flags.Enum scene_flags = SceneManager.Flags.None;
		
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
			boolean flipped = (target.isFlipped() ^ render_renderer.isTargetFlipped());
			render_manager.getDraw().drawTexture(command, texture, flipped, RenderDraw.Mode.Composite);
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
	
	private App app;							// Application interface
	private Window window;						// Window interface
	private Device device;						// Device interface
	private Target target;						// Target interface
	
	private Async main_async;					// main Async
	private Async process_async;				// process Async
	private Thread process_thread;				// process Thread
	
	private SceneManager scene_manager;			// SceneManager interface
	private Scene scene;						// Scene interface
	
	private Graph graph;						// Graph interface
	private NodeCamera node_camera;				// NodeCamera interface
	private CameraPerspective camera;			// CameraPerspective interface
	
	private NodeObject torus_node;				// NodeObject interface
	
	private RenderManager render_manager;		// RenderManager interface
	private RenderRenderer render_renderer;		// RenderRenderer interface
	private RenderSpatial render_spatial;		// RenderSpatial interface
	private RenderFrame render_frame;			// RenderFrame interface
	
	private long counter = 0;					// frame counter
}
