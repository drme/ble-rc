import Foundation;

class StatusViewControllerEx: StaticDataTableViewController
{
	@IBOutlet var mainBatteryCell: UITableViewCell!;
	@IBOutlet var receiverBatteryCell: UITableViewCell!;
	@IBOutlet var currentCell: UITableViewCell!;
	@IBOutlet var cpuTemperatureCell: UITableViewCell!;
	@IBOutlet var engineTemperatureCell: UITableViewCell!;
	@IBOutlet var accelerationCell: UITableViewCell!;
	@IBOutlet var latencyCell: UITableViewCell!;
	@IBOutlet var engineTemperatureTextField: UITextField!;
	@IBOutlet var mcuTemperatureTextField: UITextField!;
	@IBOutlet var mainBatteryVoltageTextField: UITextField!;
	@IBOutlet var mainBatteryVoltagePercentTextField: UITextField!;
	@IBOutlet var controllerBatteryVoltageField: UITextField!;
	@IBOutlet var controllerBatteryVoltagePercentField: UITextField!;
	@IBOutlet var currentConsumptionField: UITextField!;
	@IBOutlet var accelerationField: UITextField!;
	@IBOutlet var latencyField: UITextField!;
	@IBOutlet var mainBatteryProgressView: UIProgressView!;
	@IBOutlet var controllerBatteryProgressView: UIProgressView!;
	@IBOutlet var engineTemperatureProgressView: UIProgressView!;
	@IBOutlet var mcuTemperatureProgressView: UIProgressView!;
	@IBOutlet var currentConsumptionProgressView: UIProgressView!;
	@IBOutlet var titleView: UINavigationItem!;

	override func viewDidAppear(animated: Bool)
	{
		super.viewDidAppear(animated)

		let car = DevicesManager.sharedInstance.activeCar;
		let connected = car != nil;

		setHidden(self.mainBatteryCell, hidden: !(connected && car.status.battery.hasMainVoltage));
		setHidden(self.receiverBatteryCell, hidden: !connected);
		setHidden(self.currentCell, hidden: !(connected && car.status.battery.hasLoadCurrent));
		setHidden(self.cpuTemperatureCell, hidden: !connected);
		setHidden(self.engineTemperatureCell, hidden: !(connected && car.status.temperature.hasEngineTemperature));
		setHidden(self.accelerationCell, hidden: !(connected && car.status.acceleration.hasAcceleration));
		setHidden(self.latencyCell, hidden: !connected);

		if (connected)
		{
			car.status.updated = self.updateValues;
			car.status.refreshInterval = 4.0;
			car.status.type = .Full;
		}

		updateValues();
		reloadDataAnimated(false);
	}

	override func viewWillDisappear(animated: Bool)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			car.status.updated = nil;
			car.status.refreshInterval = 100.0;
			car.status.type = .Disabled;
		}

		super.viewWillDisappear(animated);
	}

	private func setHidden(view: UITableViewCell!, hidden: Bool)
	{
		if (nil != view)
		{
			cell(view, setHidden: hidden);
		}
	}

	private func updateValues()
	{
		dispatch_async(dispatch_get_main_queue(),
		{
			let car = DevicesManager.sharedInstance.activeCar;

			if (nil != car)
			{
				self.titleView.title = car.settings.name;

				self.engineTemperatureTextField.text = UtilsEx.formatFloat(car.status.temperature.engine * 10.0, suffix: " C");
				self.engineTemperatureProgressView.progress = min(car.status.temperature.engine / 100.0, 1.0);

				self.mcuTemperatureTextField.text = UtilsEx.formatFloat(car.status.temperature.mcu, suffix: " C");
				self.mcuTemperatureProgressView.progress = car.status.temperature.mcu / 100.0;

				let battery = car.status.battery;
				let maxVoltage = car.settings.battery.maxVoltage;

				self.mainBatteryVoltageTextField.text = UtilsEx.formatFloat(Float(battery.mainVoltage) / 1000.0, suffix: " V");
				self.mainBatteryProgressView.progress = min((maxVoltage > 0) ? Float(battery.mainVoltage) / Float(maxVoltage) : 0.0, 1.0);
				self.mainBatteryVoltagePercentTextField.text = String(Int(self.mainBatteryProgressView.progress * 100.0)) + "%";

				self.controllerBatteryVoltageField.text = UtilsEx.formatFloat((2.0 + Float(battery.controllerBatteryLevel) / 100.0), suffix: " V");
				self.controllerBatteryVoltagePercentField.text = String(battery.controllerBatteryLevel) + "%";
				self.controllerBatteryProgressView.progress = min(Float(battery.controllerBatteryLevel) / 100.0, 1.0);

				self.currentConsumptionField.text = UtilsEx.formatFloat(Float(car.status.battery.loadCurrent), suffix: " A");
				self.currentConsumptionProgressView.progress = min(Float(car.status.battery.loadCurrent) / 20.0, 1.0);

				let x = car.status.acceleration.x;
				let y = car.status.acceleration.y;
				let z = car.status.acceleration.z;

				self.accelerationField.text = UtilsEx.formatFloat(x, suffix: ";") + UtilsEx.formatFloat(y, suffix: ";") + UtilsEx.formatFloat(z, suffix: " G");

				self.latencyField.text = String(Int(car.status.latency * 1000.0)) + " ms";
			}
			else
			{
				self.titleView.title = "Disconnected";
				self.engineTemperatureTextField.text = "0 C";
				self.engineTemperatureProgressView.progress = 0.0;
				self.mcuTemperatureTextField.text = "0 C";
				self.mcuTemperatureProgressView.progress = 0.0;
				self.mainBatteryVoltageTextField.text = "0.0 V";
				self.mainBatteryVoltagePercentTextField.text = "0 %";
				self.mainBatteryProgressView.progress = 0.0;
				self.controllerBatteryVoltageField.text = "0.0 V";
				self.controllerBatteryVoltagePercentField.text = "0 %";
				self.controllerBatteryProgressView.progress = 0.0;
				self.currentConsumptionField.text = "0.0 A";
				self.currentConsumptionProgressView.progress = 0.0;
				self.accelerationField.text = "0;0;0 G";
				self.latencyField.text = "";
			}
		});
	}
}
