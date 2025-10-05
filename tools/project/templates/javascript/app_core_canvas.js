// Tellusim Canvas Application Template

/*
 */
Tellusim().then(ts => {
	ts.run(main);
});

/*
 */
class Application {
	
	// constructor initializing the application
	constructor(ts, app) {
		this.ts = ts;
		this.app = app;
	}
	
	// create function responsible for initialization
	create() {
		
		// create window
		if(!this.create_window()) return false;
		
		// create resources
		if(!this.create_resources()) return false;
		
		// additional initialization code can go here
		
		return true;
	}
	
	// update function for logic updates
	update() {
		
		let ts = this.ts;
		
		// update canvas elements
		let hheight = this.canvas.height * 0.5;
		for(let i = 0; i < this.ellipses.length; i++) {
			let angle = ts.Base.Pi2 * i / this.ellipses.length + ts.Time.seconds();
			let x = this.canvas.width * (1 + i) / (this.ellipses.length + 1);
			let y0 = hheight + Math.cos(angle * 2.0 - 0.2) * Math.sin(angle - 0.1) * hheight * 0.9;
			let y1 = hheight + Math.cos(angle * 2.0 + 0.2) * Math.sin(angle + 0.1) * hheight * 0.9;
			this.ellipses[i].setColor(ts.Color.hsv(angle * ts.Base.Rad2Deg + y0 * 0.5, 1.0, 1.0), 0);
			this.ellipses[i].setRadius(Math.sqrt(20.0 * 20.0 + (y1 - y0) * (y1 - y0) * 0.25));
			this.ellipses[i].setPosition0(new ts.Vector3f(x, y0, 0.0));
			this.ellipses[i].setPosition1(new ts.Vector3f(x, y1, 0.0));
		}
		
		// your update logic here
		
		return true;
	}
	
	// main run loop
	run() {
		
		// run application
		this.window.run(() => {
			return this.render_window();
		});
		
		// finish context
		this.window.finish();
		
		return true;
	}
	
	// helper function to create and initialize the Window
	create_window() {
		
		let ts = this.ts;
		
		// create Window
		this.window = new ts.Window(this.app.platform, this.app.device);
		if(!this.window.isValidPtr() || !this.window.setSize(this.app.width, this.app.height) ||
			!this.window.create(this.window.platform_name + ' JavaScript @NAME@') || !this.window.setHidden(false)) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_window(): can\'t create Window\n');
			return false;
		}
		
		// set Window callbacks
		this.window.setKeyboardPressedCallback((key, code) => {
			if(key == ts.Window.Key.Esc) this.window.stop();
		});
		this.window.setCloseClickedCallback(() => {
			this.window.stop();
		});
		
		// create Device
		this.device = new ts.Device(this.window);
		if(!this.device.isValidPtr()) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_window(): can\'t create Device\n');
			return false;
		}
		
		// create Target
		this.target = this.device.createTarget(this.window);
		if(!this.target.isValidPtr()) {
			ts.Log.print(ts.Log.Level.Error, 'Application::create_window(): can\'t create Target\n');
			return false;
		}
		
		return true;
	}
	
	// helper function to create and load resources
	create_resources() {
		
		let ts = this.ts;
		
		// gray background
		this.target.setClearColor(new ts.Color(0.1));
		
		// create canvas
		this.canvas = new ts.Canvas();
		this.canvas.setViewport(1280.0, 720.0);
		
		// create ellipses
		this.ellipses = []
		for(let i = 0; i < 32; i++) {
			let ellipse = new ts.CanvasEllipse(this.canvas);
			ellipse.setStrokeStyle(new ts.StrokeStyle(4.0, ts.Color.white()));
			this.ellipses.push(ellipse);
		}
		
		// your resource creation code here
		
		return true;
	}
	
	// helper function to render the Window
	render_window() {
		
		let ts = this.ts;
		
		// update events
		ts.Window.update();
		
		// update application
		if(!this.update()) return false;
		
		// render window
		if(!this.window.render()) return false;
		
		// create canvas
		if(!this.canvas.create(this.device, this.target)) return false;
		
		// window target
		this.target.begin();
		{
			let command = this.device.createCommand(this.target);
			
			// draw canvas
			this.canvas.draw(command, this.target);
			
			command.destroyPtr();
		}
		this.target.end();
		
		// present window
		if(!this.window.present()) return false;
		
		// check errors
		if(!this.device.check()) return false;
		
		return true;
	}
}

/*
 */
function main(ts, app) {
	
	// instantiate the application
	let application = new Application(ts, app);
	
	// create the application
	if(!application.create()) return;
	
	// run the application
	if(!application.run()) return;
}
