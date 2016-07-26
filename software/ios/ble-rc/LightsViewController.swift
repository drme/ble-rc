import UIKit

class LightsViewController: UIViewController
{
	@IBOutlet var leftTurnButton: UIButton!;
	@IBOutlet var rightTurnButton: UIButton!;
	@IBOutlet var emergencyTurnButton: UIButton!;
	@IBOutlet var lightsButton: UIButton!;
	private var leftOn = false;
	private var rightOn = false;
	private var emergencyOn = false;
	private var tick = false;
	private var lightsBlinkTimer: NSTimer!;

	override func viewWillAppear(animated: Bool)
	{
		super.viewWillAppear(animated);

		self.lightsBlinkTimer = NSTimer.scheduledTimerWithTimeInterval(2.0, target: self, selector: "updateLights", userInfo: nil, repeats: true);
	}

	override func viewWillDisappear(animated: Bool)
	{
		super.viewWillDisappear(animated);

		self.lightsBlinkTimer.invalidate();
	}

	@IBAction func lightsPressed(sender: UIButton)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			car.lights.headOn = !car.lights.headOn;
			car.lights.breakOn = !car.lights.breakOn;
		}
	}

	@IBAction func leftTurnPressed(sender: UIButton)
	{
		self.leftOn = !self.leftOn;

		if (self.leftOn == true)
		{
			self.rightOn = false;
		}
	}

	@IBAction func rightTurnPressed(sender: UIButton)
	{
		self.rightOn = !self.rightOn;

		if (self.rightOn == true)
		{
			self.leftOn = false;
		}
	}

	@IBAction func emergencyPressed(sender: UIButton)
	{
		self.emergencyOn = !self.emergencyOn;
	}

	func updateLights()
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		self.tick = !self.tick;

		if (true == self.emergencyOn)
		{
			car.lights.leftTurnOn = self.tick;
			car.lights.rightTurnOn = self.tick;
		}
		else
		{
			if (true == self.leftOn)
			{
				car.lights.leftTurnOn = self.tick;
			}
			else if (true == self.rightOn)
			{
				car.lights.rightTurnOn = self.tick;
			}
		}
	}
}
