// Tellusim Application Template

import UIKit

import Tellusim

/*
 */
class AppDelegate: UIResponder, UIApplicationDelegate {
	
	func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
		Log.print(Log.Level.Verbose, "didFinishLaunchingWithOptions(): is called\n")
		DispatchQueue.main.asyncAfter(deadline: .now() + 0.1) {
			chdir(Bundle.main.bundlePath)
			Log.printf(Log.Level.Message, "main: %d\n", main_())
		}
		return true
	}
	
	func applicationWillResignActive(_ application: UIApplication) {
		Log.print(Log.Level.Verbose, "applicationWillResignActive(): is called\n")
	}
	
	func applicationDidEnterBackground(_ application: UIApplication) {
		Log.print(Log.Level.Verbose, "applicationDidEnterBackground(): is called\n")
	}
	
	func applicationWillEnterForeground(_ application: UIApplication) {
		Log.print(Log.Level.Verbose, "applicationWillEnterForeground(): is called\n")
	}
	
	func applicationDidBecomeActive(_ application: UIApplication) {
		Log.print(Log.Level.Verbose, "applicationDidBecomeActive(): is called\n")
	}
}
