import Foundation;
import CoreBluetooth;

class Lights
{
	private let settings: LightsSettings;
	private var lightsProxy = CharacteristicProxy(shortId: "a101");
	private var lightsState: UInt16 = 0;
	
	init(inout characteristics: [CharacteristicProxy], settings: LightsSettings)
	{
		self.settings = settings;

		characteristics.append(self.lightsProxy);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		switch(type)
		{
			case .MiniZBLDC:
				return [self.lightsProxy.getId(long)];
			case .MiniZ:
				return [];
			default:
				return [self.lightsProxy.getId(long)];
		}
	}

	func updateLights(value: Bool, bit: UInt16)
	{
		let mask = UInt16(1 << bit);

		if (value)
		{
			self.lightsState |= mask;
		}
		else
		{
			self.lightsState &= ~mask;
		}

		self.lightsProxy.value = self.lightsState;
	}

	var headOn: Bool = false
	{
		didSet(value)
		{
			updateLights(value, bit: self.settings.mainNo);
		}
	}

	var leftTurnOn: Bool = false
	{
		didSet(value)
		{
			updateLights(value, bit: self.settings.leftNo);
		}
	}

	var rightTurnOn: Bool = false
	{
		didSet(value)
		{
			updateLights(value, bit: self.settings.rightNo);
		}
	}

	var reverseOn: Bool = false
	{
		willSet(newValue)
		{
			if (newValue != self.reverseOn)
			{
				updateLights(newValue, bit: self.settings.reverseNo);
			}
		}
	}

	var breakOn: Bool = false
	{
		didSet(value)
		{
			updateLights(value, bit: self.settings.backNo);
		}
	}
}
