import Foundation;
import CoreBluetooth;

class TemperatureStatus
{
	private var mcuTemperatureProxy = CharacteristicProxy(shortId: "3002");
	private var engineTemperatureProxy = CharacteristicProxy(shortId: "3001");

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.mcuTemperatureProxy);
		characteristics.append(self.engineTemperatureProxy);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		switch(type)
		{
			case .MiniZBLDC, .MiniZ:
				return [self.mcuTemperatureProxy.getId(long)];
			default:
				return [self.mcuTemperatureProxy.getId(long), self.engineTemperatureProxy.getId(long)];
		}
	}

	func refresh() -> Int
	{
		var reads = 0;

		reads += self.mcuTemperatureProxy.startValueRead();
		reads += self.engineTemperatureProxy.startValueRead();

		return reads;
	}

	var hasEngineTemperature: Bool
	{
		return self.engineTemperatureProxy.available;
	}

	var engine: Float
	{
		return Float(self.engineTemperatureProxy.value) / 100.0;
	}

	var mcu: Float
	{
		return Float(self.mcuTemperatureProxy.value) / 100.0;
	}
}
