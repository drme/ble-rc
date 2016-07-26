import Foundation
import CoreBluetooth

class CharacteristicProxy
{
	let shortId: CBUUID;
	private let longId: CBUUID;
	private(set) var characteristic: CBCharacteristic!;
	private var device: CBPeripheral!;
	private var integerValue: UInt16;
	private let size: Int;
	private var textValue = "";
	private var readOnDiscover = false;
	var stopWrites = false;
	var updated: Event!;
	let required: Bool;

	init(shortId: String, size: Int = 2, initialValue: UInt16 = 0, readOnDiscover: Bool = false, required: Bool = false)
	{
		self.shortId = CBUUID(string: shortId);
		self.longId = CharacteristicProxy.getLongId(shortId);
		self.integerValue = initialValue;
		self.size = size;
		self.readOnDiscover = readOnDiscover;
		self.required = required;
	}

	init(longId: String, size: Int = 2, initialValue: UInt16 = 0, readOnDiscover: Bool = false)
	{
		self.shortId = CBUUID(string: longId);
		self.longId = CBUUID(string: longId);
		self.integerValue = initialValue;
		self.size = size;
		self.readOnDiscover = readOnDiscover;
		self.required = false;
	}

	var available: Bool
	{
		return (nil != self.characteristic);
	}

	var value: UInt16
	{
		get
		{
			return self.integerValue;
		}
		set(newValue)
		{
			self.integerValue = newValue;

			if ((nil != self.characteristic) && (false == self.stopWrites))
			{
				var data = self.integerValue;

				NSLog("Sending data for %@", self.characteristic.UUID);

				self.device.writeValue(NSData(bytes: &data, length: 2), forCharacteristic: self.characteristic, type: .WithoutResponse);
			}
		}
	}

	var boolValue: Bool!
	{
		get
		{
			return nil;
		}
		set(newValue)
		{
			if ((nil != self.characteristic) && (false == self.stopWrites))
			{
				NSLog("Sending data for %@", self.characteristic.UUID);

				var configurationData: UInt8 = (true == newValue) ? 1 : 0;

				self.device.writeValue(NSData(bytes: &configurationData, length: 1), forCharacteristic: self.characteristic, type: .WithResponse);
			}
		}
	}

	var stringValue: String
	{
		get
		{
			return self.textValue;
		}
		set(value)
		{
			self.textValue = value;

			if (nil != self.characteristic)
			{
				var stringData = [UInt8](count: 20, repeatedValue: 0x00);

				let newValue: [UInt8] = Array(value.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet()).utf8);

				for (var i = 0; (i < newValue.count && i < 20); i++)
				{
					stringData[i] = newValue[i];
				}

				let data = NSData(bytes: &stringData, length: 20);

				self.device.writeValue(data, forCharacteristic: self.characteristic, type: .WithoutResponse);
			}
		}
	}

	func getId(long: Bool) -> CBUUID
	{
		if (true == long)
		{
			return self.longId;
		}
		else
		{
			return self.shortId;
		}
	}

	func disconnect()
	{
		self.characteristic = nil;
		self.device = nil;
		self.textValue = "";
		self.integerValue = 0;
	}

	func attach(characteristic: CBCharacteristic, device: CBPeripheral) -> Bool
	{
		if ((characteristic.UUID == self.longId) || (characteristic.UUID == self.shortId))
		{
			self.characteristic = characteristic;
			self.device = device;

			if (true == self.readOnDiscover)
			{
				startValueRead();
			}

			return true;
		}

		return false;
	}

	class func getLongId(shortId: String) -> CBUUID
	{
		return CBUUID(string: "a739" + shortId + "-f6cd-1692-994a-d66d9e0ce048");
	}

	func startValueRead() -> Int
	{
		if (nil != self.characteristic)
		{
			self.device.readValueForCharacteristic(self.characteristic);

			return 1;
		}

		return 0;
	}

	func resendValue()
	{
		self.value = self.integerValue;
	}

	func updateValue(characteristic: CBCharacteristic) -> Bool
	{
		if (self.characteristic === characteristic)
		{
			if (self.size == 2)
			{
				var temp: UInt16 = 0;

				self.characteristic.value?.getBytes(&temp, length: 2);

				self.integerValue = temp;

				NSLog("Updating data for %@ %d", self.characteristic.UUID, temp);

				if (nil != self.updated)
				{
					self.updated();
				}

				return true;
			}
			else if (self.size > 19)
			{
				if (self.characteristic.value != nil)
				{
					NSLog("Updating data for %@", self.characteristic.UUID);

					var datastring: String! = NSString(data: self.characteristic.value!, encoding:NSUTF8StringEncoding) as! String;

					if (nil != datastring)
					{
						datastring = datastring?.stringByReplacingOccurrencesOfString("\0", withString: "");
						datastring = datastring?.stringByTrimmingCharactersInSet(NSCharacterSet.whitespaceAndNewlineCharacterSet());

						self.textValue = datastring;
					}
					else
					{
						self.textValue = "";
					}
				}

				if (nil != self.updated)
				{
					self.updated();
				}
				
				return true;
			}
			else
			{
				NSLog("Updating data for %@", self.characteristic.UUID);

				if (nil != self.updated)
				{
					self.updated();
				}
			}
		}

		return false;
	}
}
