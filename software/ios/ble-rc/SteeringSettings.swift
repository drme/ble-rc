import Foundation;
import CoreBluetooth;

class SteeringSettings
{
	private var maxSteeringProxy = CharacteristicProxy(shortId: "c102", size: 2, initialValue: 550, readOnDiscover: true, required: true);
	private var minSteeringProxy = CharacteristicProxy(shortId: "c101", size: 2, initialValue: 200, readOnDiscover: true, required: true);
	private var centerSteeringProxy = CharacteristicProxy(shortId: "c103", size: 2, initialValue: 375, readOnDiscover: true, required: true);
	private let useAccelerometerKey	= "useAccelerometer";
	private let centerSteeringKey = "centerSteering";
	private let accelerometerAxisKey = "accelerometerAxis";
	private let invertSteeringKey = "invertSteering";

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.maxSteeringProxy);
		characteristics.append(self.minSteeringProxy);
		characteristics.append(self.centerSteeringProxy);

		self.useAccelerometer = NSUserDefaults.standardUserDefaults().boolForKey(self.useAccelerometerKey);
		self.inverted = NSUserDefaults.standardUserDefaults().boolForKey(self.invertSteeringKey);
		self.symmetric = NSUserDefaults.standardUserDefaults().boolForKey(self.centerSteeringKey);

		self.accelerometerAxis = NSUserDefaults.standardUserDefaults().integerForKey(self.accelerometerAxisKey);

		if ((self.accelerometerAxis <= 0) || (self.accelerometerAxis > 2))
		{
			self.accelerometerAxis = 1;
		}
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		return [self.centerSteeringProxy.getId(long), self.minSteeringProxy.getId(long), self.maxSteeringProxy.getId(long)];
	}

	var max: UInt16
	{
		get
		{
			return self.maxSteeringProxy.value;
		}
		set(value)
		{
			self.maxSteeringProxy.value = value;
		}
	}

	var min: UInt16
	{
		get
		{
			return self.minSteeringProxy.value;
		}
		set(value)
		{
			self.minSteeringProxy.value = value;
		}
	}

	var center: UInt16
	{
		get
		{
			return self.centerSteeringProxy.value;
		}
		set(value)
		{
			self.centerSteeringProxy.value = value;
		}
	}

	var useAccelerometer: Bool = false
	{
		willSet(value)
		{
			UtilsEx.saveValue(self.useAccelerometerKey, value: value);
		}
	}

	var inverted: Bool = false
	{
		willSet(value)
		{
			UtilsEx.saveValue(self.invertSteeringKey, value: value);
		}
	}

	var symmetric: Bool = false
	{
		willSet(value)
		{
			UtilsEx.saveValue(self.centerSteeringKey, value: value);
		}
	}

	var accelerometerAxis: Int = 1
	{
		willSet(value)
		{
			let userDefaults = NSUserDefaults.standardUserDefaults();

			if (value != userDefaults.integerForKey(self.accelerometerAxisKey))
			{
				userDefaults.setInteger(value, forKey: self.accelerometerAxisKey);

				userDefaults.synchronize();
			}
		}
	}
}
