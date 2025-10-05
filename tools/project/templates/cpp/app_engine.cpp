// Tellusim Engine SDK Application Template

#include <TellusimApp.h>
#include <core/TellusimCore.h>
#include <core/TellusimThread.h>
#include <platform/TellusimPlatforms.h>
#include <scene/TellusimScenes.h>
#include <scene/TellusimCameras.h>
#include <scene/TellusimNodes.h>
#include <scene/TellusimGraph.h>

#if _ANDROID
	#include <system/TellusimAndroid.h>
#endif

#include <render/system/include/TellusimRenderSystem.h>

/*
 */
using namespace Tellusim;

/*
 */
#define LOAD_SCENE	1

/*
 */
class Application {
		
	public:
		
		// constructor initializing the application with command-line arguments
		explicit Application(int32_t argc, char **argv) : app(argc, argv) {
			
		}
		
		// destructor cleans up resources
		~Application() {
			
			// stop process thread
			if(process_thread) {
				scene_manager.terminate();
				process_thread->stop(true);
			}
			
			// finish device
			if(device) device.finish();
			
			// release render
			render_frame.clearPtr();
			render_system.clear();
			
			// clear Scene
			if(scene) scene.clear();
			if(scene_manager) scene_manager.update(device, main_async);
			if(device) device.finish();
			
			// release Scene
			scene.clearPtr();
			scene_manager.clearPtr();
			
			TS_LOG(Verbose, "Application::~Application(): Done\n");
		}
		
		// create function responsible for initialization
		bool create() {
			
			// create app
			if(!app.create()) {
				TS_LOG(Error, "Application::create(): can't create App\n");
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
				stream.loadGraph(scene, "scene.graphx", [&](uint32_t stream_id, Graph graph) {
					TS_LOG(Message, "Application::create(): scene is loaded\n");
					torus_node = NodeObject(graph.getNode("Torus Node"));
				}, SceneStream::DefaultFlags);
			#endif
			
			// additional initialization code can go here
			
			return true;
		}
		
		// update function for logic updates
		bool update() {
			
			// update scene
			#if LOAD_SCENE
				float32_t angle = (float32_t)Time::seconds() * 16.0f;
				node_camera.setGlobalTransform(Matrix4x3d::placeTo(Matrix4x3d::rotateZ(-angle) * Vector3d(2.0, 2.0, 1.0), Vector3d::zero, Vector3d(0.0, 0.0, 1.0)));
				node_camera.updateScene();
				if(torus_node) {
					torus_node.setPivotTransform(Matrix4x3f::rotateX(angle * 3.0f) * Matrix4x3f::rotateZ(angle * 2.0f));
					torus_node.updateScene();
				}
			#endif
			
			// your update logic here
			
			return true;
		}
		
		// main run loop
		bool run() {
			
			// run application
			if(window) return window.run([this]() {
				return render_window();
			});
			
			return true;
		}
		
	private:
		
		// helper function to create and initialize Window
		bool create_window() {
			
			// create Window
			window = Window(app.getPlatform(), app.getDevice());
			if(!window || !window.setSize(app.getWidth(), app.getHeight()) ||
				!window.create(String(window.getPlatformName()) + " @NAME@") || !window.setHidden(false)) {
				TS_LOG(Error, "Application::create_window(): can't create Window\n");
				return false;
			}
			
			// set Window callbacks
			window.setKeyboardPressedCallback([this](uint32_t key, uint32_t code) {
				if(key == Window::KeyF4 && window.getKeyboardKey(Window::KeyAlt)) window.stop();
				if(key == Window::KeyEsc) window.stop();
			});
			window.setCloseClickedCallback([&]() { window.stop(); });
			
			// create Device
			device = Device(window);
			if(!device) {
				TS_LOG(Error, "Application::create_window(): can't create Device\n");
				return false;
			}
			
			// create Target
			target = device.createTarget(window);
			if(!target) {
				TS_LOG(Error, "Application::create_window(): can't create Target\n");
				return false;
			}
			
			return true;
		}
		
		// helper function to create and initialize Scene
		bool create_scene(SceneManager::Flags flags = SceneManager::DefaultFlags) {
			
			// main async
			if(!main_async.init()) {
				TS_LOGE(Error, "Application::create_scene(): can't initialize main Async\n");
				return false;
			}
			
			// process async
			if(!process_async.init()) {
				TS_LOGE(Error, "Application::create_scene(): can't initialize process Async\n");
				return false;
			}
			
			// cache location
			#if _ANDROID
				SceneManager::setShaderCache(Android::getCacheDirectory() + "/shader.cache");
				SceneManager::setTextureCache(Android::getCacheDirectory() + "/texture.cache");
			#else
				SceneManager::setShaderCache("shader.cache");
				SceneManager::setTextureCache("texture.cache");
			#endif
			
			// scene formats
			#if _ANDROID || _IOS
				scene_manager.setCubeTextureParameters(FormatRGBAu8n, 512, 32, 2);
				scene_manager.setEnvironmentParameters(device, 128, 32, 512, 1024);
			#endif
			
			TS_LOG(Verbose, "Application::create_scene(): create SceneManager\n");
			
			// create scene manager
			if(!scene_manager.create(device, flags, nullptr, &main_async)) {
				TS_LOG(Error, "Application::create_scene(): can't create SceneManager\n");
				return false;
			}
			
			TS_LOG(Verbose, "Application::create_scene(): SceneManager is ready\n");
			
			// create process thread
			process_thread = makeAutoPtr(makeThreadFunction([this](Thread *thread) {
				bool done = !scene_manager.process(process_async);
				if(done) Time::sleep(1000);
			}));
			
			// run process thread
			if(!process_thread->run()) {
				TS_LOGE(Error, "Application::create_scene(): can't run process Thread\n");
				return false;
			}
			
			// initialize scene
			scene.setManager(scene_manager);
			scene.addGraph(graph);
			scene.updateGraph(graph);
			
			// create camera
			scene.addCamera(camera);
			graph.addNode(node_camera);
			node_camera.setCamera(camera);
			
			return true;
		}
		
		// helper function to create and initialize Render
		bool create_render(RenderRenderer::Flags flags = RenderRenderer::FlagAntialiasing | RenderRenderer::FlagMotionBuffer | RenderRenderer::FlagScreenOcclusion | RenderRenderer::FlagScreenReflection) {
			
			TS_LOG(Verbose, "Application::create_render(): create RenderSystem\n");
			
			// create render system
			render_system = makeAutoPtr(new RenderSystem(scene_manager));
			if(!render_system->create(device, window.getSurface(), flags)) {
				TS_LOG(Error, "Application::create_render(): can't create manager\n");
				return false;
			}
			
			TS_LOG(Verbose, "Application::create_render(): RenderSystem is ready\n");
			
			// create render frame
			render_frame = RenderFrame(render_system->getManager(), node_camera);
			
			// scene render
			scene.setRender(render_system->getRender());
			
			return true;
		}
		
		// helper function to render the Window
		bool render_window(SceneManager::Flags scene_flags = SceneManager::FlagNone, RenderSystem::Flags render_flags = RenderSystem::DefaultFlags, uint64_t usec = 10000) {
			
			// update events
			Window::update();
			
			// update application
			if(!update()) return false;
			
			// render Window
			if(!window.render()) return false;
			
			// resize RenderFrame with 200% upscale
			uint32_t width = window.getWidth();
			uint32_t height = window.getHeight();
			Texture texture = render_frame.getCompositeTexture();
			if(!texture || texture.getWidth() != width || texture.getHeight() != height) {
				if(!device.finish()) return false;
				render_frame.create(device, render_system->getRenderer(), width / 2, height / 2, Size(width, height));
			}
			
			// create Scene
			if(!scene.create(device, &main_async)) {
				TS_LOG(Error, "Application::render_window(): can't create Scene\n");
				return false;
			}
			
			// update Scene
			scene.setTime(Time::seconds());
			scene.update(device);
			
			// update SceneManager
			if(!scene_manager.update(device, main_async, counter++, 0, usec, scene_flags)) {
				TS_LOG(Error, "Application::render_window(): can't update SceneManager\n");
				return false;
			}
			
			// dispatch SceneManager
			{
				Compute compute = device.createCompute();
				
				// dispatch SceneManager
				scene_manager.dispatch(device, compute);
				
				// dispatch Scene
				scene.dispatch(device, compute);
			}
			
			// render frame
			if(!render_system->render(device, render_frame, render_flags)) {
				TS_LOG(Error, "Application::render_window(): can't render RenderSystem\n");
				return false;
			}
			
			// window target
			target.begin();
			{
				Command command = device.createCommand(target);
				Texture texture = render_frame.getCompositeTexture();
				bool flipped = (target.isFlipped() ^ render_system->isFlipped());
				render_system->getDraw().drawTexture(command, texture, flipped, RenderDraw::ModeComposite);
			}
			target.end();
			
			// present Window
			if(!window.present()) return false;
			
			// check errors
			if(!device.check()) return false;
			
			return true;
		}
		
		App app;								// Application interface
		Window window;							// Window interface
		Device device;							// Device interface
		Target target;							// Target interface
		
		Async main_async;						// main Async
		Async process_async;					// process Async
		AutoPtr<Thread> process_thread;			// process Thread
		
		SceneManager scene_manager;				// SceneManager interface
		Scene scene;							// Scene interface
		
		Graph graph;							// Graph interface
		NodeCamera node_camera;					// NodeCamera interface
		CameraPerspective camera;				// Camera interface
		
		#if LOAD_SCENE
			NodeObject torus_node;				// torus node
		#endif
		
		AutoPtr<RenderSystem> render_system;	// RenderSystem plugin
		RenderFrame render_frame;				// RenderFrame interface
		
		uint64_t counter = 0;					// frame counter
};

/*
 */
int32_t main(int32_t argc, char **argv) {
	
	// instantiate the application
	Application application(argc, argv);
	
	// create the application
	if(!application.create()) return 1;
	
	// run the application
	if(!application.run()) return 1;
	
	return 0;
}

/*
 */
#if _WIN32
	#include <system/TellusimWindows.h>
	TS_DECLARE_WINDOWS_HIGH_PERFORMANCE
#elif _ANDROID
	#include <system/TellusimAndroid.h>
	TS_DECLARE_ANDROID_NATIVE_ACTIVITY
#endif
