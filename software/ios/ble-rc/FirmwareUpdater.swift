import Foundation;
import CoreBluetooth;

typealias Event = () -> (Void);
typealias EventProgress = (percent: Float, secondsLeft: Float) -> Void;
typealias EventBadImage = (type: UInt8) -> Void;

enum FirmwareUpdaterState
{
	case Ready;
	case Flashing;
	case Canceled;
	case WaitingForReboot;
}

class FirmwareFile
{
	let fileName: String;
	let displayName: String;
	let imageType: UInt16;

	init(fileName: String, name: String, type: UInt16)
	{
		self.fileName = fileName;
		self.displayName = name;
		self.imageType = type;
	}
}

class FirmwareUpdater
{
	static let tiOadServiceId = CBUUID(string: "F000FFC0-0451-4000-B000-000000000000");
	static let imageNotifyId = CBUUID(string: "F000FFC1-0451-4000-B000-000000000000");
	static let imageBlockId = CBUUID(string: "F000FFC2-0451-4000-B000-000000000000");
	static let deviceInformationServiceId	= CBUUID(string: "180a");
	static let deviceHardwareRevisionId = CBUUID(string: "2a27");
	static let deviceFirmwareRevisionId = CBUUID(string: "2a26");
	private (set) var firmwareVersion = "";
	private (set) var hardwareVersion = "";
	private let oadBlockSize = 16;
	private let OAD_IMG_ID_SIZE = 4;
	private let OAD_IMG_HDR_SIZE = (2 + 2 + 4);
	private let HAL_FLASH_WORD_SIZE = 4;
	private var availableFirmwareFiles: [FirmwareFile] = [];
	private var imageNotifyCharacteristic: CBCharacteristic!;
	private var imageBlockCharacteristic: CBCharacteristic!;
	private var deviceFirmwareVersionCharacteristic: CBCharacteristic!;
	private var	deviceHardwareVersionCharacteristic: CBCharacteristic!;
	private var device: CBPeripheral!;
	private var totallBlocks: UInt16 = 0;
	private (set) var currentImageVersion: UInt16 = 0xffff;
	private var imageFile: NSData! = nil;
	private var failedHandler: Event?;
	private var succeededHandler: Event?;
	private var disconnectedHandler: Event!;
	private var badImageHandler: EventBadImage!;
	private var progressHandler: EventProgress?;
	private var startTime: NSDate!;
	private (set) var state: FirmwareUpdaterState = .Ready;
	private var expectedImageVersion: UInt16 = 0xffff;

	init(device: CBPeripheral!)
	{
		self.device = device;

		loadFirmwareFiles();
	}

	func assignService(service: CBService)
	{
		for characteristic in service.characteristics!
		{
			assignCharacteristic(characteristic);
		}
	}

	private func assignCharacteristic(characteristic: CBCharacteristic) -> Bool
	{
		if (characteristic.UUID == FirmwareUpdater.imageNotifyId)
		{
			self.imageNotifyCharacteristic = characteristic;

			self.device.setNotifyValue(true, forCharacteristic: characteristic);

			var data: UInt8 = 0x00;

			self.device.writeValue(NSData(bytes: &data, length: 1), forCharacteristic: characteristic, type: .WithResponse);

			self.currentImageVersion = 0xFFFF;

			dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(1.5 * Double(NSEC_PER_SEC))), dispatch_get_main_queue())
			{
				var data: UInt8 = 0x01;

				self.device.writeValue(NSData(bytes: &data, length: 1), forCharacteristic: self.imageNotifyCharacteristic, type: .WithResponse);
			}
		}
		else if (characteristic.UUID == FirmwareUpdater.imageBlockId)
		{
			self.imageBlockCharacteristic = characteristic;

			self.device.setNotifyValue(true, forCharacteristic: characteristic);
		}
		else if (characteristic.UUID == FirmwareUpdater.deviceFirmwareRevisionId)
		{
			self.deviceFirmwareVersionCharacteristic = characteristic;
			self.device.readValueForCharacteristic(characteristic);
		}
		else if (characteristic.UUID == FirmwareUpdater.deviceHardwareRevisionId)
		{
			self.deviceHardwareVersionCharacteristic = characteristic;
			self.device.readValueForCharacteristic(characteristic);
		}
		else
		{
			return false;
		}

		return true;
	}

	func filterFirmwareFiles(accept: (file: FirmwareFile) -> Bool) -> [FirmwareFile]
	{
		var filteredFirmwareFiles: [FirmwareFile] = [];

		for file in self.availableFirmwareFiles
		{
			if ((self.currentImageVersion & 0x01) != (file.imageType & 0x01))
			{
				if (accept(file: file))
				{
					filteredFirmwareFiles.append(file);
				}
			}
		}

		return filteredFirmwareFiles;
	}

	private func isCorrectImage(imageFile: NSData) -> Bool
	{
		var crc0: UInt16 = 0;
		var crc1: UInt16 = 0;
		var ver: UInt16 = 0;
		var len: UInt16 = 0;

		imageFile.getBytes(&crc0, range: NSRange(location: 0, length: 2));
		imageFile.getBytes(&crc1, range: NSRange(location: 2, length: 2));
		imageFile.getBytes(&ver, range: NSRange(location: 4, length: 2));
		imageFile.getBytes(&len, range: NSRange(location: 6, length: 2));

		NSLog("CRC0: 0x%x, CRC1: 0x%x, VER: %d, LEN: %d", crc0, crc1, ver, len);

		if ((ver & 0x01) != (self.currentImageVersion & 0x01))
		{
			return true;
		}

		return false;
	}

	private func loadFirmwareFiles()
	{
		do
		{
			let resourcePath = NSBundle.mainBundle().resourcePath;

			for file in try NSFileManager.defaultManager().contentsOfDirectoryAtPath(resourcePath!)
			{
				if (true == file.hasSuffix(".bin"))
				{
					var imageType: UInt16 = 0xffff;

					if (file.containsString("-a-"))
					{
						imageType = 0;
					}
					else if (file.containsString("-b-"))
					{
						imageType = 1;
					}
					else
					{
						continue;
					}

					self.availableFirmwareFiles.append(FirmwareFile(fileName: resourcePath! + "/" + file, name: file, type: imageType));
				}
			}
		}
		catch
		{
		}
	}

	func startUpdate(firmwareFile: FirmwareFile, failedHandler: Event, succeededHandler: Event, disconnectedHandler: Event, badImageHandler: EventBadImage, progressHandler: EventProgress)
	{
		self.failedHandler = failedHandler;
		self.succeededHandler = succeededHandler;
		self.disconnectedHandler = disconnectedHandler;
		self.badImageHandler = badImageHandler;
		self.progressHandler = progressHandler;

		self.imageFile = NSData(contentsOfFile: firmwareFile.fileName);

		NSLog("Loaded firmware \"%@\"of size : %lu", firmwareFile.fileName, self.imageFile.length);

		if (isCorrectImage(self.imageFile))
		{
			uploadImage(firmwareFile.fileName);
		}
		else
		{
			if (nil != self.badImageHandler)
			{
				self.badImageHandler(type: UInt8(self.currentImageVersion & 0x01));
			}
		}
	}

	func didUpdateValueForProfile(characteristic: CBCharacteristic) -> Bool
	{
		if (characteristic === self.imageNotifyCharacteristic)
		{
			if (self.currentImageVersion == 0xFFFF)
			{
				if (characteristic.value?.length >= 2)
				{
					var data: [UInt8] = [UInt8](count: (characteristic.value?.length)!, repeatedValue: 0x00);

					characteristic.value?.getBytes(&data, length: data.count);

					self.currentImageVersion = (UInt16(data[1]) << 8 & 0xff00) | (UInt16(data[0]) & 0xff);

					NSLog("self.imgVersion : %04hx", self.currentImageVersion);

					if (self.expectedImageVersion != 0xffff)
					{
						if (self.expectedImageVersion == self.currentImageVersion)
						{
							NSLog("Firmware check after reboot: correct image is running");
							self.succeededHandler?();
						}
						else
						{
							NSLog("Firmware check after reboot: wrong image is running");
							self.failedHandler?();
						}

						self.state = .Ready;
					}
				}
				else
				{
					NSLog("Invalid image version size %d", (characteristic.value?.length)!);
				}
			}

			NSLog("OAD Image notify : %@", characteristic.value!);

			return true;
		}
		else if (characteristic === self.imageBlockCharacteristic)
		{
			var data: [UInt8] = [UInt8](count: characteristic.value!.length, repeatedValue: 0x00);

			characteristic.value!.getBytes(&data, length:characteristic.value!.length);

			let number : UInt16 = (UInt16(data[1]) << 8 & 0xff00) | (UInt16(data[0]) & 0xff);

			if (self.state == .Flashing)
			{
				sendBlock(number);
			}

			return true;
		}
		if (characteristic == self.deviceFirmwareVersionCharacteristic)
		{
			self.firmwareVersion = readString(characteristic);

			return true;
		}
		else if (characteristic == self.deviceHardwareVersionCharacteristic)
		{
			self.hardwareVersion = readString(characteristic);

			return true;
		}

		return false;
	}

	private func hi(a: UInt16) -> UInt8
	{
		return UInt8(((a) >> 8) & 0xff);
	}

	private func lo(a: UInt16) -> UInt8
	{
		return UInt8((a) & 0xff);
	}

	private func sendBlock(requestedBlock: UInt16)
	{
		if (nil == self.imageFile)
		{
			return;
		}

		if (self.state != .Flashing)
		{
			return;
		}

		var requestData = [UInt8](count: 2 + self.oadBlockSize, repeatedValue: 0);

		requestData[0] = lo(requestedBlock);
		requestData[1] = hi(requestedBlock);

		NSLog("Sending block %x %x %d", requestData[0], requestData[1], requestedBlock);

		var buf = [UInt8](count: self.oadBlockSize, repeatedValue: 0xfe);

		self.imageFile.getBytes(&buf, range: NSRange(location: self.oadBlockSize * Int(requestedBlock), length: self.oadBlockSize));

		for (var i = 0; i < self.oadBlockSize; i++)
		{
			requestData[2 + i] = buf[i];
		}

		if (nil != self.device)
		{
			let data = NSData(bytes: requestData, length: requestData.count);

//			NSLog("%04hx %@", requestedBlock, data.hexString);

			if (requestedBlock == self.totallBlocks - 1)
			{
				self.state = .WaitingForReboot;
			}

			self.device.writeValue(data, forCharacteristic:self.imageBlockCharacteristic, type: .WithoutResponse);
		}

		if (requestedBlock == self.totallBlocks - 1)
		{
			let timeInterval: Double = NSDate().timeIntervalSinceDate(self.startTime) / 60.0;

			print("Time to update: \(timeInterval) min");

			dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(10 * Double(NSEC_PER_SEC))), dispatch_get_main_queue())
			{
				if (self.state == .WaitingForReboot)
				{
					self.failedHandler?();
				}
			}

			return;
		}

		updateProgress(requestedBlock);
	}

	private func updateProgress(currentBlock: UInt16)
	{
		let secondsPerBlock: Float = 0.10;
		let secondsLeft = Float(self.totallBlocks - currentBlock) * secondsPerBlock;

		self.progressHandler?(percent: Float(currentBlock) / Float(self.totallBlocks), secondsLeft: secondsLeft);
	}

	func uploadImage(fileName: String)
	{
		self.startTime = NSDate();
		self.state = .Flashing;

		var requestData = [UInt8](count: OAD_IMG_HDR_SIZE + 2 + 2, repeatedValue: 0);

		var crc0: UInt16 = 0;
		var crc1: UInt16 = 0;
		var ver: UInt16 = 0;
		var len: UInt16 = 0;
		var uid: [UInt8] = [0, 0, 0, 0];

		imageFile.getBytes(&crc0, range: NSRange(location: 0, length: 2));
		imageFile.getBytes(&crc1, range: NSRange(location: 2, length: 2));
		imageFile.getBytes(&ver, range: NSRange(location: 4, length: 2));
		imageFile.getBytes(&len, range: NSRange(location: 6, length: 2));
		imageFile.getBytes(&uid, range: NSRange(location: 8, length: 4));

		NSLog("CRC0: 0x%x, CRC1: 0x%x, VER: %d, LEN: %d uid: %x%x%x%x", crc0, crc1, ver, len, uid[0], uid[1], uid[2], uid[3]);

		requestData[0] = lo(ver);
		requestData[1] = hi(ver);

		requestData[2] = lo(len);
		requestData[3] = hi(len);

		NSLog("Image version = %04hx, len = %04hx", ver, len);

		self.expectedImageVersion = ver;

		for (var i = 0; i < 4; i++)
		{
			requestData[4 + i] = uid[i];
		}

		requestData[OAD_IMG_HDR_SIZE + 0] = lo(12);
		requestData[OAD_IMG_HDR_SIZE + 1] = hi(12);

		requestData[OAD_IMG_HDR_SIZE + 2] = lo(15);
		requestData[OAD_IMG_HDR_SIZE + 1] = hi(15);

		NSLog("RD: %x %x %x %x %x %x %x %x %x %x %x %x", requestData[0], requestData[1], requestData[2], requestData[3], requestData[4], requestData[5], requestData[6], requestData[7], requestData[8], requestData[9], requestData[10], requestData[11]);

		if (nil != self.device)
		{
			let data = NSData(bytes: requestData, length: OAD_IMG_HDR_SIZE + 2 + 2);

			NSLog("%04hx %@", -1, data.hexString);

			self.device.writeValue(data, forCharacteristic:self.imageNotifyCharacteristic, type:.WithResponse);
		}

		self.totallBlocks = UInt16(Int(len) / (self.oadBlockSize / HAL_FLASH_WORD_SIZE));

		updateProgress(0);
	}

	func cancelUpdate()
	{
		self.state = .Canceled;
	}

	func disconnect()
	{
		self.firmwareVersion = "";
		self.hardwareVersion = "";
		self.imageBlockCharacteristic = nil;
		self.imageNotifyCharacteristic = nil;
		self.totallBlocks = 0;
		self.imageFile = nil;
		self.currentImageVersion = 0xffff;
	}

	func deviceDisconnected(peripheral: CBPeripheral)
	{
		if (peripheral === self.device)
		{
			if (self.state == .Flashing)
			{
				cancelUpdate();
				self.failedHandler?();
			}

			disconnect();
		}
	}

	private func readString(characteristic: CBCharacteristic) -> String
	{
		let data: NSData! = characteristic.value;

		if (nil != data)
		{
			let result: String! = String(data: data, encoding: NSUTF8StringEncoding);

			if (nil != result)
			{
				return result;
			}
		}

		return "";
	}
}

class SimulatedFirmwareUpdater: FirmwareUpdater
{
	private var progress = 0;

	init()
	{
		super.init(device: nil);
	}

	override func uploadImage(fileName: String)
	{
		self.progress = 0;

		let timer = NSTimer.scheduledTimerWithTimeInterval(0.1, target: self, selector: "simulate:", userInfo: nil, repeats: true);

		timer.fire();
	}

	@objc func simulate(timer: NSTimer)
	{
		self.progress++;

		self.progressHandler?(percent: Float(self.progress) / 100.0, secondsLeft: Float(100 - self.progress));

		if (self.progress > 100)
		{
			timer.invalidate();
			self.succeededHandler?();
		}
	}
}
