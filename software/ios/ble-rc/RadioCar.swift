import Foundation;
import CoreBluetooth;

@objc public class RadioCar: NSObject, CBPeripheralDelegate
{
	static let carServiceIdShort = CBUUID(string: "acc0");
	static let carServiceIdLong = CharacteristicProxy.getLongId("acc0");
	private(set) var device: CBPeripheral!
	private var throttleProxy = CharacteristicProxy(shortId: "acc1", required: true);
	private var steeringProxy = CharacteristicProxy(shortId: "acc2", required: true);
	private var allDriveProxy = CharacteristicProxy(shortId: "acc3", size: 4);
	private var resendTimer: NSTimer!;
	private var characteristics = [CharacteristicProxy]();
	var comment: String = "";
	private(set) var firmwareUpdater: FirmwareUpdater;
	var reconnect = true;
	let settings: Settings;
	let status: Status;
	let lights: Lights;

	init(data peripheral: CBPeripheral)
	{
		self.settings = Settings(characteristics: &self.characteristics, device: device, fakeName: nil);
		self.status = Status(characteristics: &self.characteristics);
		self.lights = Lights(characteristics: &self.characteristics, settings: self.settings.lights);
		self.firmwareUpdater = FirmwareUpdater(device: peripheral);

		super.init();

		self.device = peripheral;
		self.device.delegate = self;

		self.characteristics.append(self.steeringProxy);
		self.characteristics.append(self.throttleProxy);
		self.characteristics.append(self.allDriveProxy);
	}

	init(name: String, comment: String)
	{
		self.settings = Settings(characteristics: &self.characteristics, device: nil, fakeName: name);
		self.status = Status(characteristics: &self.characteristics);
		self.lights = Lights(characteristics: &self.characteristics, settings: self.settings.lights);
		self.firmwareUpdater = SimulatedFirmwareUpdater();
		self.settings.type = .MiniZ;

		super.init();

		self.device = nil;
		self.comment = comment;
	}

	func resendValues()
	{
		if (self.firmwareUpdater.state != .Flashing)
		{
			if ((self.allDriveProxy.available) && (nil != self.device))
			{
				var v2 = [ self.throttleProxy.value, self.steeringProxy.value ];

			//	NSLog("resend...");
				self.device.writeValue(NSData(bytes: &v2, length: 4), forCharacteristic: self.allDriveProxy.characteristic, type: .WithoutResponse);

				self.steeringProxy.stopWrites = true;
				self.throttleProxy.stopWrites = true;
			}
			else
			{
				self.steeringProxy.resendValue();
				self.throttleProxy.resendValue();
			}
		}
	}

	func startServicesSeach()
	{
		self.reconnect = true;

		for proxy in self.characteristics
		{
			proxy.disconnect();
		}

		self.firmwareUpdater.disconnect();

		self.device.discoverServices([RadioCar.carServiceIdLong, RadioCar.carServiceIdShort, BatteryStatus.batteryServiceId, AccelerationStatus.accelerometerServiceId, FirmwareUpdater.deviceInformationServiceId, FirmwareUpdater.tiOadServiceId]);
	}

	func disconnect()
	{
		self.reconnect = false;

	/*	

		if (nil != self.resendTimer)
		{
			self.resendTimer.invalidate();
			self.resendTimer = nil;
		}

		enableAccelerometer(false); */
	}

	var lostConnection = false
	{
		didSet
		{
			if ((self.lostConnection) && (nil != self.device))
			{
				self.firmwareUpdater.deviceDisconnected(self.device);
			}

			self.status.updated?();
		}
	}

	var enabled: Bool = false
	{
		didSet
		{
			if (true == self.enabled)
			{
			}
			else
			{
				self.throttle = self.settings.throttle.center;
				self.steering = self.settings.steering.center;

				self.resendTimer.invalidate();
				self.resendTimer = nil;
			}
		}
	}

	var steering: UInt16
	{
		get
		{
			return self.steeringProxy.value;
		}
		set(value)
		{
			if (abs(Int(self.steering) - Int(value)) > 1)
			{
				self.steeringProxy.value = value;
			}
		}
	}

	var throttle: UInt16
	{
		get
		{
			return self.throttleProxy.value;
		}
		set(value)
		{
			if (nil == self.resendTimer)
			{
				self.resendTimer = NSTimer.scheduledTimerWithTimeInterval(0.03, target: self, selector: "resendValues", userInfo: nil, repeats: true);
				self.resendTimer.fire();
			}

			if (value != self.throttle)
			{
				self.throttleProxy.value = value;

				self.lights.reverseOn = self.throttle < self.settings.throttle.center;
			}
		}
	}

	public func peripheral(peripheral: CBPeripheral, didUpdateValueForCharacteristic characteristic: CBCharacteristic, error: NSError?)
	{
/*		if (nil != error)
		{
			NSLog("Error %@", error!.localizedDescription);

			return;
		}
*/
		if (true == self.firmwareUpdater.didUpdateValueForProfile(characteristic))
		{
//			return;
		}
/*

		else if (characteristic === self.centerSteeringProxy.characteristic)
		{
			self.steering = UInt16(self.steeringCenter);
		}
		else if (characteristic === self.centerThrottleProxy.characteristic)
		{
			self.throttle = UInt16(self.throttleCenter);
		}

*/
		for proxy in self.characteristics
		{
			if (proxy.updateValue(characteristic))
			{
//				return;
			}
		}
/*
		NSLog("Unhandled characteristic value read: %@ with ID %@", characteristic.value!, characteristic.UUID); */
	}

	public func peripheral(peripheral: CBPeripheral, didDiscoverCharacteristicsForService service: CBService, error: NSError?)
	{
		if ((nil != error) || (nil == service.characteristics))
		{
			return;
		}

		self.firmwareUpdater.assignService(service);

		for characteristic in service.characteristics!
		{
			var found = false;

			for proxy in self.characteristics
			{
				found = found || proxy.attach(characteristic, device: peripheral);
			}

			if (false == found)
			{
				NSLog("-----Unhandled characteristic: %@", characteristic.UUID);
			}
		}

		var missing = false;

		for proxy in self.characteristics
		{
			if ((proxy.required) && (false == proxy.available))
			{
				NSLog("Missing: %@", proxy.shortId);
				missing = true;
				break;
			}
		}

		if (false == missing)
		{
			self.lostConnection = false;
			DevicesManager.sharedInstance.raiseCarFullyConnected(self);
		}
	}

	private func buildCharacteristicsList(long: Bool) -> [CBUUID]!
	{
		var result: [CBUUID] = [self.throttleProxy.getId(long), self.steeringProxy.getId(long), self.allDriveProxy.getId(long)];

		result += self.settings.getRequiredIds(long, type: self.settings.type);
		result += self.status.getRequiredIds(long, type: self.settings.type);
		result += self.lights.getRequiredIds(long, type: self.settings.type);

		return result;
	}

	public func peripheral(peripheral: CBPeripheral, didDiscoverServices error: NSError?)
	{
		if ((nil != error) || (peripheral.services == nil))
		{
			return;
		}

		for service in (peripheral.services)!
		{
			if (service.UUID == BatteryStatus.batteryServiceId)
			{
				peripheral.discoverCharacteristics([self.status.battery.controllerBatteryVoltageProxy.shortId], forService: service);
			}
			else if (service.UUID == FirmwareUpdater.deviceInformationServiceId)
			{
				peripheral.discoverCharacteristics([FirmwareUpdater.deviceFirmwareRevisionId, FirmwareUpdater.deviceHardwareRevisionId], forService: service);
			}
			else if (service.UUID == FirmwareUpdater.tiOadServiceId)
			{
				peripheral.discoverCharacteristics([FirmwareUpdater.imageBlockId, FirmwareUpdater.imageNotifyId], forService: service);
			}
			else if (service.UUID == RadioCar.carServiceIdLong)
			{
				peripheral.discoverCharacteristics(buildCharacteristicsList(true), forService: service);
			}
			else if (service.UUID == RadioCar.carServiceIdShort)
			{
				peripheral.discoverCharacteristics(buildCharacteristicsList(false), forService: service);
			}
			else if (service.UUID == AccelerationStatus.accelerometerServiceId)
			{
				peripheral.discoverCharacteristics(self.status.acceleration.getRequiredIds(self.settings.type), forService: service);
			}
		}
	}
}
