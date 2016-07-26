import Foundation;
import CoreBluetooth;

class ThrottleSettings
{
	private var minThrottleProxy = CharacteristicProxy(shortId: "b101", size:2, initialValue: 200, readOnDiscover: true, required: true);
	private var maxThrottleProxy = CharacteristicProxy(shortId: "b102", size:2, initialValue: 550, readOnDiscover: true, required: true);
	private var centerThrottleProxy = CharacteristicProxy(shortId: "b103", size:2, initialValue: 375, readOnDiscover: true, required: true);
	private var pulseWidthProxy = CharacteristicProxy(shortId: "d101", size:2, initialValue: 375, readOnDiscover: false);
	private let invertThrottleKey = "invertThrottle";

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.minThrottleProxy);
		characteristics.append(self.maxThrottleProxy);
		characteristics.append(self.centerThrottleProxy);
		characteristics.append(self.pulseWidthProxy);

		self.inverted = NSUserDefaults.standardUserDefaults().boolForKey(self.invertThrottleKey);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		return [self.centerThrottleProxy.getId(long), self.maxThrottleProxy.getId(long), self.minThrottleProxy.getId(long)];
	}

	var max: UInt16
	{
		get
		{
			return self.maxThrottleProxy.value;
		}
		set(value)
		{
			self.maxThrottleProxy.value = value;
		}
	}

	var min: UInt16
	{
		get
		{
			return self.minThrottleProxy.value;
		}
		set(value)
		{
			self.minThrottleProxy.value = value;
		}
	}

	var center: UInt16
	{
		get
		{
			return self.centerThrottleProxy.value;
		}
		set(value)
		{
			self.centerThrottleProxy.value = value;
		}
	}

	var inverted: Bool = false
	{
		willSet(value)
		{
			UtilsEx.saveValue(self.invertThrottleKey, value: value);
		}
	}
}
