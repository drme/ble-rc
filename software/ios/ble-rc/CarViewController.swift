import Foundation;
import CoreMotion;

class CarViewControllerEx: UIViewController, JoyStickDelegate
{
	@IBOutlet var throttleSlider: JoyStickView!;
	@IBOutlet var steeringSlider: JoyStickView!;
	@IBOutlet var titleView: UINavigationItem!;
	@IBOutlet var backgroundView: UIImageView!;
	@IBOutlet var bottomBar: UIView!;
	@IBOutlet var batteryImage: UIImageView!;
	@IBOutlet var connectionStatusView: UIImageView!;
	private var batteryImages = [ UIImage(named: "Battery-0"), UIImage(named: "Battery-25"), UIImage(named: "Battery-50"), UIImage(named: "Battery-75"), UIImage(named: "Battery-100") ];
	private var connectionImages = [ UIImage(named: "ConnectionOffIcon"), UIImage(named: "ConnectionOnIcon"), UIImage(named: "ConnectionSlowIcon") ];
	private var enabled = false;
	private var pressed = false;
	private let motionManager = CMMotionManager();

	override func viewDidLoad()
	{
		super.viewDidLoad();

		self.bottomBar.backgroundColor = AppSettings.applicationTint;
		self.connectionStatusView.transform = CGAffineTransformMakeRotation(CGFloat(M_PI / 2.0));
		self.batteryImage.transform = CGAffineTransformMakeRotation(CGFloat(M_PI / 2.0));
		self.backgroundView?.image = makeBackgroundImage("Background");

		self.motionManager.accelerometerUpdateInterval = 0.02;

		self.motionManager.startAccelerometerUpdatesToQueue(NSOperationQueue.currentQueue()!, withHandler:
		{
			(accelerometerData, error) in

			let connectedCar = DevicesManager.sharedInstance.activeCar;

			if ((nil != connectedCar) && (true == connectedCar.settings.steering.useAccelerometer))
			{
				switch (connectedCar.settings.steering.accelerometerAxis)
				{
					case 0:
						self.updateSteering(accelerometerData!.acceleration.x);
						break;
					case 2:
						self.updateSteering(accelerometerData!.acceleration.z);
						break;
					default:
						self.updateSteering(accelerometerData!.acceleration.y);
						break;
				}
			}
		});

		self.throttleSlider!.setDelegate(self);
		self.steeringSlider!.setDelegate(self);

		self.steeringSlider!.vertical = true;
	}

	private func updateValues()
	{
		let car = DevicesManager.sharedInstance.activeCar;

		self.batteryImage.hidden = (car == nil);
		self.connectionStatusView.hidden = (car == nil);

		if (nil != car)
		{
			if (car.status.battery.controllerBatteryLevel > 90)
			{
				self.batteryImage.image = self.batteryImages[4];
			}
			else if (car.status.battery.controllerBatteryLevel > 70)
			{
				self.batteryImage.image = self.batteryImages[3];
			}
			else if (car.status.battery.controllerBatteryLevel > 55)
			{
				self.batteryImage.image = self.batteryImages[2];
			}
			else if (car.status.battery.controllerBatteryLevel > 30)
			{
				self.batteryImage.image = self.batteryImages[1];
			}
			else
			{
				self.batteryImage.image = self.batteryImages[0];
			}

			if (true == car.lostConnection)
			{
				self.connectionStatusView.image = self.connectionImages[0];
			}
			else if (car.status.latency < 0.029)
			{
				self.connectionStatusView.image = self.connectionImages[1];
			}
			else
			{
				self.connectionStatusView.image = self.connectionImages[2];
			}

			self.titleView.title = car.settings.name;
		}
	}

	@IBAction func disconnectPressed(sender: AnyObject)
	{
		DevicesManager.sharedInstance.disconnectActive();
	}

	private func startDriwing()
	{
		if (false == self.enabled)
		{
			UIView.animateWithDuration(1.0, animations:
			{
				self.tabBarController!.tabBar.alpha = 0;
				self.throttleSlider!.alpha = 1;

				let connectedCar = DevicesManager.sharedInstance.activeCar;

				if ((nil != connectedCar) && (false == connectedCar.settings.steering.useAccelerometer))
				{
					self.steeringSlider!.alpha = 1.0;
					self.steeringSlider!.enabled = true;
				}
				else
				{
					self.steeringSlider!.alpha = 0.5;
					self.steeringSlider!.enabled = false;
				}

				self.throttleSlider!.enabled = true;
			});

			UIApplication.sharedApplication().setStatusBarHidden(true, withAnimation:.Slide);

			self.navigationController!.setNavigationBarHidden(true, animated:true);

			let connectedCar = DevicesManager.sharedInstance.activeCar;

			if (nil != connectedCar)
			{
				connectedCar.enabled = true;
			}

			self.enabled = true;
		}
	}

	private func stopDriwing()
	{
		if (true == self.enabled)
		{
			UIView.animateWithDuration(1.0, animations:
			{
				self.tabBarController!.tabBar.alpha = 1;
				self.throttleSlider!.alpha = 0.5;
				self.steeringSlider!.alpha = 0.5;
			});

			UIApplication.sharedApplication().setStatusBarHidden(false, withAnimation: .Slide);

			self.navigationController!.setNavigationBarHidden(false, animated: true);

			let connectedCar = DevicesManager.sharedInstance.activeCar;

			if (nil != connectedCar)
			{
				connectedCar.enabled = false;
			}

			self.steeringSlider!.enabled = false;
			self.throttleSlider!.enabled = false;
					
			self.enabled = false;
		}
	}

	private func onThrottleChanged(pos: Float)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		startDriwing();

		if (true == car.enabled)
		{
			self.pressed = fabs(pos) >= 0.000001;

			var velocityVector = pos / 2.0;

			if (car.settings.throttle.inverted)
			{
				velocityVector *= -1.0;
			}

			if (true == self.pressed)
			{
				let nds = Float(car.settings.throttle.center) - Float(car.settings.throttle.min);
				let pds = Float(car.settings.throttle.max) - Float(car.settings.throttle.center);

				car.throttle = UInt16(Float(car.settings.throttle.center) + ((velocityVector < 0.0) ? (nds * velocityVector * 2.0) : (pds * velocityVector * 2.0)));
			}
			else
			{
				car.throttle = car.settings.throttle.center;
			}
		}
		else
		{
			car.throttle = car.settings.throttle.center;
		}
	}

	private func onSteeringChanged(pos: Float)
	{
		let connectedCar = DevicesManager.sharedInstance.activeCar;

		if (nil == connectedCar)
		{
			return;
		}

		startDriwing();

		if (true == connectedCar.settings.steering.useAccelerometer)
		{
			return;
		}
			
		updateSteering(Double(pos));
	}

	private func updateSteering(value: Double)
	{
		let connectedCar = DevicesManager.sharedInstance.activeCar;

		if (nil == connectedCar)
		{
			return;
		}

		if (true == self.enabled)
		{
			var v = value;

			if (v < -0.5)
			{
				v = -0.5;
			}

			if (v > 0.5)
			{
				v = 0.5;
			}

			if (fabs(v) < 0.1)
			{
				v = 0;
			}

			if (connectedCar.settings.steering.inverted)
			{
				v *= -1.0;
			}

			let nds = Double(connectedCar.settings.steering.center) - Double(connectedCar.settings.steering.min);
			let pds = Double(connectedCar.settings.steering.max) - Double(connectedCar.settings.steering.center);

			connectedCar.steering = UInt16(Double(connectedCar.settings.steering.center) + ((v < 0.0) ? (nds * v * 2.0) : (pds * v * 2.0)));
		}
	}

	override func viewDidAppear(animated: Bool)
	{
		self.enabled = false;

		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			car.enabled = true;
			car.status.refreshInterval = 10.0;
			car.status.updated = self.updateValues;
			car.status.type = .Minimal;
		}

		super.viewDidAppear(animated);
	}

	override func viewWillDisappear(animated: Bool)
	{
		self.enabled = false;

		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			car.enabled = false;
			car.status.refreshInterval = 10.0;
			car.status.updated = nil;
			car.status.type = .Disabled;
		}

		super.viewWillDisappear(animated);
	}

	@IBAction func powerPressed(sender: AnyObject)
	{
		stopDriwing();
	}

	private func makeBackgroundImage(imageName: String) -> UIImage
	{
		let sourceImage = UIImage(named: imageName)!;

		let imageSize = CGSize(width: self.view.bounds.size.width * 2, height: self.view.bounds.size.height * 2);

		UIGraphicsBeginImageContext(imageSize);

		let context = UIGraphicsGetCurrentContext();

		let scale = sourceImage.size.width / sourceImage.size.height;

		CGContextDrawImage(context, CGRectMake(0, 0, imageSize.width, imageSize.height * scale), sourceImage.CGImage);

		let outputImage = UIGraphicsGetImageFromCurrentImageContext();

		UIGraphicsEndImageContext();

		let size = outputImage.size;

		UIGraphicsBeginImageContext(CGSizeMake(size.height, size.width));

		UIImage(CGImage: outputImage.CGImage!, scale: 1.0, orientation: .Left).drawInRect(CGRectMake(0,0,size.height ,size.width));

		let newImage = UIGraphicsGetImageFromCurrentImageContext();

		UIGraphicsEndImageContext();

		return newImage;
	}

	func onStickChanged(sender: JoyStickView!, _ pos: Float)
	{
		if (sender === self.throttleSlider)
		{
			onThrottleChanged(pos);
		}
		else
		{
			onSteeringChanged(pos);
		}
	}
}
