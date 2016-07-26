import Foundation;
import CoreBluetooth;

class BatterySettings
{
	private var batteryMaxVoltageProxy = CharacteristicProxy(shortId: "e103", size:2, initialValue: 0, readOnDiscover: true);
	private var batteryCapacityProxy = CharacteristicProxy(shortId: "e104", size: 2, initialValue: 750, readOnDiscover: true);
	private var batteryCalibrationProxy = CharacteristicProxy(shortId: "e106", size: 2, initialValue: 0, readOnDiscover: true);
	private var batteryCurrentLimitProxy = CharacteristicProxy(shortId: "e107", size: 2, initialValue: 0, readOnDiscover: true);
	private var batteryMinVoltageProxy = CharacteristicProxy(shortId: "e108", size: 2, initialValue: 0, readOnDiscover: true);

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.batteryMaxVoltageProxy);
		characteristics.append(self.batteryCapacityProxy);
		characteristics.append(self.batteryCalibrationProxy);
		characteristics.append(self.batteryCurrentLimitProxy);
		characteristics.append(self.batteryMinVoltageProxy);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		switch(type)
		{
			case .MiniZBLDC:
				return [self.batteryMaxVoltageProxy.getId(long), self.batteryCapacityProxy.getId(long), self.batteryMinVoltageProxy.getId(long)];
			case .MiniZ:
				return [];
			default:
				return [self.batteryMaxVoltageProxy.getId(long), self.batteryCapacityProxy.getId(long), self.batteryCalibrationProxy.getId(long), self.batteryCurrentLimitProxy.getId(long), self.batteryMinVoltageProxy.getId(long)];
		}
	}

	var hasCurrent: Bool
	{
		return self.batteryCalibrationProxy.available && self.batteryCurrentLimitProxy.available;
	}

	var hasVoltage: Bool
	{
		return self.batteryMaxVoltageProxy.available && self.batteryCapacityProxy.available && self.batteryMinVoltageProxy.available;
	}

	var maxVoltage: UInt16
	{
		get
		{
			return self.batteryMaxVoltageProxy.value;
		}
		set(value)
		{
			self.batteryMaxVoltageProxy.value = value;
		}
	}

	var capacity: UInt16
	{
		get
		{
			return self.batteryCapacityProxy.value;
		}
		set(value)
		{
			self.batteryCapacityProxy.value = value;
		}
	}

	var minVoltage: UInt16 = 0;
	var maxCurrent: UInt16 = 0;
	var currentCalibration: UInt16 = 0;
}
