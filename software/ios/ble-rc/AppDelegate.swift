import Foundation
import UIKit

@UIApplicationMain class AppDelegate: UIResponder, UIApplicationDelegate
{
	var window: UIWindow?

	func application(application: UIApplication, didFinishLaunchingWithOptions launchOptions: [NSObject: AnyObject]?) -> Bool
	{
		self.window?.tintColor = AppSettings.applicationTint

		UITabBar.appearance().tintColor = AppSettings.applicationTint
		UINavigationBar.appearance().barTintColor = AppSettings.applicationTint
		UINavigationBar.appearance().titleTextAttributes = [NSForegroundColorAttributeName : UIColor.whiteColor()]
		UINavigationBar.appearance().tintColor = UIColor.whiteColor()
		UISwitch.appearance().onTintColor = AppSettings.applicationTint

		return true
	}

	func applicationWillResignActive(application: UIApplication)
	{
	}

	func applicationDidEnterBackground(application: UIApplication)
	{
	}

	func applicationWillEnterForeground(application: UIApplication)
	{
	}

	func applicationDidBecomeActive(application: UIApplication)
	{
	}

	func applicationWillTerminate(application: UIApplication)
	{
	}
}
