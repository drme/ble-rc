import Foundation;
import CoreBluetooth;

class LightsSettings
{
	private var mainLightsNoProxy = CharacteristicProxy(shortId: "1001", size: 2, initialValue: 0, readOnDiscover: true);
	private var backLightsNoProxy = CharacteristicProxy(shortId: "1002", size: 2, initialValue: 1, readOnDiscover: true);
	private var reverseLightsNoProxy = CharacteristicProxy(shortId: "1003", size: 2, initialValue: 2, readOnDiscover: true);
	private var leftLightsNoProxy = CharacteristicProxy(shortId: "1004", size: 2, initialValue: 3, readOnDiscover: true);
	private var rightLightsNoProxy = CharacteristicProxy(shortId: "1005", size: 2, initialValue: 4, readOnDiscover: true);

	init(inout characteristics: [CharacteristicProxy])
	{
		characteristics.append(self.mainLightsNoProxy);
		characteristics.append(self.backLightsNoProxy);
		characteristics.append(self.reverseLightsNoProxy);
		characteristics.append(self.leftLightsNoProxy);
		characteristics.append(self.rightLightsNoProxy);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		switch(type)
		{
			case .MiniZ:
				return [];
			default:
				return [self.mainLightsNoProxy.getId(long), self.backLightsNoProxy.getId(long), self.reverseLightsNoProxy.getId(long), self.leftLightsNoProxy.getId(long), self.rightLightsNoProxy.getId(long)];
		}
	}

	var hasLights: Bool
	{
		return self.mainLightsNoProxy.available;
	}

	var mainNo: UInt16
	{
		get
		{
			return self.mainLightsNoProxy.value;
		}
		set(value)
		{
			self.mainLightsNoProxy.value = value;
		}
	}

	var backNo: UInt16
	{
		get
		{
			return self.backLightsNoProxy.value;
		}
		set(value)
		{
			self.backLightsNoProxy.value = value;
		}
	}

	var reverseNo: UInt16
	{
		get
		{
			return self.reverseLightsNoProxy.value;
		}
		set(value)
		{
			self.reverseLightsNoProxy.value = value;
		}
	}

	var leftNo: UInt16
	{
		get
		{
			return self.leftLightsNoProxy.value;
		}
		set(value)
		{
			self.leftLightsNoProxy.value = value;
		}
	}

	var rightNo: UInt16
	{
		get
		{
			return self.rightLightsNoProxy.value;
		}
		set(value)
		{
			self.rightLightsNoProxy.value = value;
		}
	}
}
