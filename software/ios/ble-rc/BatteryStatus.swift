import Foundation;
import CoreBluetooth;

class BatteryStatus
{
	static let batteryServiceId = CBUUID(string: "180f");
	private var batteryVoltageProxy = CharacteristicProxy(shortId: "e101");
	private var batteryCurrentProxy = CharacteristicProxy(shortId: "e102");
	private (set) var controllerBatteryVoltageProxy = CharacteristicProxy(shortId: "2a19");

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.batteryVoltageProxy);
		characteristics.append(self.batteryCurrentProxy);
		characteristics.append(self.controllerBatteryVoltageProxy);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		switch(type)
		{
			case .MiniZBLDC:
				return [self.batteryVoltageProxy.getId(long)];
			case .MiniZ:
				return [];
			default:
				return [self.batteryVoltageProxy.getId(long), self.batteryCurrentProxy.getId(long)];
		}
	}

	func refresh(controllerOnly: Bool = false) -> Int
	{
		var reads = self.controllerBatteryVoltageProxy.startValueRead();

		if (false == controllerOnly)
		{
			reads += self.batteryCurrentProxy.startValueRead();
			reads += self.batteryVoltageProxy.startValueRead();
		}

		return reads;
	}

	var hasMainVoltage: Bool
	{
		return self.batteryVoltageProxy.available;
	}

	var hasLoadCurrent: Bool
	{
		return self.batteryCurrentProxy.available;
	}

	var mainVoltage: UInt16
	{
		return self.batteryVoltageProxy.value;
	}

	var controllerBatteryLevel: UInt16
	{
		return self.controllerBatteryVoltageProxy.value;
	}

	var loadCurrent: UInt16
	{
		return self.batteryCurrentProxy.value;
	}
}
