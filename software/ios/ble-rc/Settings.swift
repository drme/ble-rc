import Foundation;
import CoreBluetooth;

class Settings
{
	let steering: SteeringSettings;
	let throttle: ThrottleSettings;
	let lights: LightsSettings;
	let battery: BatterySettings;
	private let device: CBPeripheral!;
	private var advertisementName: String!;
	private var fakeName: String!;
	var type: RadioCarType = .Unknown;
	private var nameProxy = CharacteristicProxy(shortId: "f101", size: 20, readOnDiscover: true);
	private var deviceCapabilitiesProxy = CharacteristicProxy(shortId: "5001", readOnDiscover: true);

	init(inout characteristics: [CharacteristicProxy], device: CBPeripheral!, fakeName: String!)
	{
		characteristics.append(self.nameProxy);
		characteristics.append(self.deviceCapabilitiesProxy);

		self.fakeName = fakeName;
		self.device = device;
		self.steering = SteeringSettings(characteristics: &characteristics);
		self.throttle = ThrottleSettings(characteristics: &characteristics);
		self.lights = LightsSettings(characteristics: &characteristics);
		self.battery = BatterySettings(characteristics: &characteristics);
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		var ids: [CBUUID] = [];

		ids += self.steering.getRequiredIds(long, type: type);
		ids += self.throttle.getRequiredIds(long, type: type)
		ids += self.lights.getRequiredIds(long, type: type)
		ids += self.battery.getRequiredIds(long, type: type);

		ids += [self.deviceCapabilitiesProxy.getId(long), self.nameProxy.getId(long)];

		return ids;
	}

	var name: String
	{
		get
		{
			if ((self.nameProxy.available) && (self.nameProxy.stringValue.characters.count > 0))
			{
				return self.nameProxy.stringValue;
			}
			else if ((nil != self.advertisementName) && (self.advertisementName.characters.count > 0))
			{
				return self.advertisementName;
			}
			else if ((nil != self.device) && (nil != self.device.name) && (self.device.name?.characters.count > 0))
			{
				return self.device.name!;
			}
			else if ((nil != self.fakeName) && (self.fakeName.characters.count > 0))
			{
				return self.fakeName;
			}
			else
			{
				return "Buggy";
			}
		}
		set(value)
		{
			if (self.nameProxy.available)
			{
				NSLog("Setting name: [%@]", value);
				self.nameProxy.stringValue = value;
			}
		}
	}

	func parseName(advertisementData: [String: AnyObject])
	{
		self.advertisementName = advertisementData["kCBAdvDataLocalName"] as! String!;

		let data = advertisementData["kCBAdvDataManufacturerData"] as! NSData!;

		if ((nil != data) && (data.length == 3))
		{
			var type: [UInt8] = [0, 0, 0];

			data.getBytes(&type, length: 3);

			if let carType = RadioCarType(rawValue: type[2])
			{
				self.type = carType;
			}
		}
	}
}
