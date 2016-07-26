import Foundation;
import CoreBluetooth;

class AccelerationStatus
{
	static let accelerometerServiceId = CBUUID(string: "f000aa10-0451-4000-b000-000000000000");
	private var accelerometerEnableProxy = CharacteristicProxy(longId: "f000aa12-0451-4000-b000-000000000000", size: 1, initialValue: 0);
	private var accelerometerValueProxy = CharacteristicProxy(longId: "f000aa11-0451-4000-b000-000000000000", size: 8, initialValue: 0);
	private var accelerometerUpdatePeriodProxy = CharacteristicProxy(longId: "f000aa13-0451-4000-b000-000000000000", size: 8, initialValue: 0);
	private (set) var x: Float = 0;
	private (set) var y: Float = 0;
	private (set) var z: Float = 0;

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.accelerometerValueProxy);
		characteristics.append(self.accelerometerEnableProxy);
		characteristics.append(self.accelerometerUpdatePeriodProxy);

		self.accelerometerValueProxy.updated = self.parseValues;
	}

	func getRequiredIds(type: RadioCarType) -> [CBUUID]
	{
		switch(type)
		{
			case .MiniZBLDC, .MiniZ:
				return [];
			default:
				return [self.accelerometerEnableProxy.shortId, self.accelerometerValueProxy.shortId, self.accelerometerUpdatePeriodProxy.shortId];
		}
	}

	func refresh() -> Int
	{
		if (self.enabled)
		{
			return self.accelerometerValueProxy.startValueRead();
		}

		return 0;
	}

	var hasAcceleration: Bool
	{
		return self.accelerometerValueProxy.available;
	}

	var enabled: Bool = false
	{
		didSet
		{
			self.accelerometerEnableProxy.boolValue = self.enabled;
		}
	}

	private func parseValues()
	{
		var temp: [Int8] = [0, 0, 0];

		self.accelerometerValueProxy.characteristic.value?.getBytes(&temp, length: 3);

		self.x = Float(temp[0]) / 64.0;
		self.y = Float(temp[1]) / 64.0;
		self.z = Float(temp[2]) / 64.0;
	}
}
