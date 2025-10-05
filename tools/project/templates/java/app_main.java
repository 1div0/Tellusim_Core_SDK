// Tellusim Application Template

package com.main;

import com.tellusim.*;

/*
 */
public class main {
	
	/*
	 */
	static {
		Base.loadDebug();
	}
	
	/*
	 */
	public static void main(String[] args) {
		
		// instantiate the application
		Application application = new Application(args);
		
		// create the application
		if(!application.create()) return;
		
		// run the application
		if(!application.run()) return;
	}
}
