import Foundation;
import CoreBluetooth;

@objc class DevicesManager: NSObject, CBCentralManagerDelegate
{
	static let sharedInstance = DevicesManager();
	private (set) var activeCar: RadioCar!;
	var radioCars = [RadioCar]();
	private var centralManager: CBCentralManager!;
	var searchEnded: Event?;
	var bluetoothTurnedOff: Event?;
	var bluetoothTurnedOn: Event?;
	var deviceFound: Event?;
	var connected: Event?;
	var disconnected: Event?;

	override init()
	{
		super.init();

		let options: [String: AnyObject] = [ CBCentralManagerOptionShowPowerAlertKey : Int(false) ];

		self.centralManager = CBCentralManager(delegate: self, queue: nil, options: options);
	}

	func raiseCarFullyConnected(car: RadioCar)
	{
		self.activeCar = car;

		self.connected?();
	}

	func connect(car: RadioCar)
	{
		if (nil == car.device)
		{
			NSLog("Connecting to fake peripheral with name : %@", car.settings.name);

			raiseCarFullyConnected(car);
		}
		else
		{
			NSLog("Connecting to peripheral with UUID : %@", car.device.identifier.UUIDString);

			self.centralManager.connectPeripheral(car.device, options: nil);
		}
	}

	func disconnectActive()
	{
		if (nil != self.activeCar)
		{
			self.activeCar.disconnect();

			if (nil != self.activeCar.device)
			{
				self.centralManager.cancelPeripheralConnection(self.activeCar.device);
			}

			self.activeCar = nil;
		}

		self.disconnected?();
	}

	func cancelConnection(car: RadioCar)
	{
		if (car.device != nil)
		{
			car.reconnect = false;
			self.centralManager.cancelPeripheralConnection(car.device);
		}
	}

	private func getCar(peripheral: CBPeripheral) -> RadioCar
	{
		for car in self.radioCars
		{
			if (peripheral === car.device)
			{
				return car;
			}
		}

		let car = RadioCar(data: peripheral);

		self.radioCars.append(car);

		return car;
	}

	private func addFakeCar(name: String) -> RadioCar
	{
		let fakeCar = RadioCar(name: name, comment: "Simulated demo car.");

		self.radioCars.append(fakeCar);

		self.deviceFound?();

		return fakeCar;
	}

	func addFakes()
	{
		addFakeCar("BMW M3 GT2");//.comment = "1:10 RC";
		addFakeCar("Ferrari F430");//.comment = "Mini-Z";
		addFakeCar("Lamborghini Murcielago SV");//.comment = "dNano";
		addFakeCar("Audi R8 LMS");//.comment = "Simple toy";
		addFakeCar("Aston Martin DBR9");//.comment = "Mini-Z BLDC";
	}

	func removeFakes()
	{
		var toRemove = [Int]();

		for (var i = self.radioCars.count - 1; i >= 0; i--)
		{
			if (nil == self.radioCars[i].device)
			{
				toRemove.append(i);
			}
		}

		for i in toRemove
		{
			self.radioCars.removeAtIndex(i);
		}

		self.deviceFound?();
	}

	func centralManager(central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: NSError?)
	{
		NSLog("Lost connection to: %@", peripheral.name!);

		if (nil != self.activeCar)
		{
			self.activeCar.lostConnection = true;

			self.centralManager.connectPeripheral(peripheral, options: nil);
		}
	}

	func centralManager(central: CBCentralManager, didConnectPeripheral peripheral: CBPeripheral)
	{
		NSLog("Connection to peripheral with UUID : %@ successfull.", peripheral.identifier.UUIDString);

		getCar(peripheral).startServicesSeach();
	}

	func centralManagerDidUpdateState(central: CBCentralManager)
	{
		switch(central.state)
		{
			case .PoweredOn:
				bluetoothTurnedOn?();
				break;
			default:
				self.radioCars.removeAll();
				bluetoothTurnedOff?();
				break;
		}
	}

	func centralManager(central: CBCentralManager, didDiscoverPeripheral peripheral: CBPeripheral, advertisementData: [String : AnyObject], RSSI: NSNumber)
	{
		let car = getCar(peripheral);

		car.settings.parseName(advertisementData);

		self.deviceFound?();
	}

	func startDeviceSearch()
	{
		disconnectActive();

		self.radioCars.removeAll();

		self.deviceFound?();

		if (self.centralManager.state != .PoweredOn)
		{
			searchEnded?();
		}
		else
		{
			dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(15.0 * Double(NSEC_PER_SEC))), dispatch_get_main_queue())
			{
				self.stopScan();
			}

			let options: [String : AnyObject] = [CBCentralManagerScanOptionAllowDuplicatesKey : Int(false)];

			self.centralManager.scanForPeripheralsWithServices([RadioCar.carServiceIdLong, RadioCar.carServiceIdShort], options: options)
		}
	}

	func stopScan()
	{
		self.centralManager.stopScan();
		searchEnded?();
	}
}
