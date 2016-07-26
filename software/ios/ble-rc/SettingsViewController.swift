import Foundation;

class SettingsViewControllerEx: StaticDataTableViewController
{
	@IBOutlet var powerCell: UITableViewCell!;
	@IBOutlet var lightsCell: UITableViewCell!;
	@IBOutlet var throttleCell: UITableViewCell!;
	@IBOutlet var steeringCell: UITableViewCell!;
	@IBOutlet var nameCell: UITableViewCell!;
	@IBOutlet var steeringAccelerometerAxisCell: UITableViewCell!;

	@IBOutlet var steeringMinValue: UITextField!;
	@IBOutlet var steeringMaxValue: UITextField!;
	@IBOutlet var steeringCenterValue: UITextField!;
	@IBOutlet var steeringMinSlider: UISlider!;
	@IBOutlet var steeringMaxSlider: UISlider!;
	@IBOutlet var steeringCenterSlider: UISlider!;
	@IBOutlet var symetricalSteering: UISwitch!;
	@IBOutlet var useAccelerometer: UISwitch!;
	@IBOutlet var invertSteering: UISwitch!;
	@IBOutlet var steeringAccelerometerAxis: UISegmentedControl!;

	@IBOutlet var throttleMinValue: UITextField!;
	@IBOutlet var throttleMaxValue: UITextField!;
	@IBOutlet var throttleCenterValue: UITextField!;
	@IBOutlet var throttleMinSlider: UISlider!;
	@IBOutlet var throttleMaxSlider: UISlider!;
	@IBOutlet var throttleCenterSlider: UISlider!;
	@IBOutlet var invertThrottle: UISwitch!;
	@IBOutlet var throttleCenterCell: UITableViewCell!;

	@IBOutlet var mainLights: UISegmentedControl!;
	@IBOutlet var backLights: UISegmentedControl!;
	@IBOutlet var reverseLights: UISegmentedControl!;
	@IBOutlet var leftLights: UISegmentedControl!;
	@IBOutlet var rightLights: UISegmentedControl!;

	@IBOutlet var carNameValue: UITextField!;

	@IBOutlet var batteryVoltageValue: UITextField!;
	@IBOutlet var batteryCapacityValue: UITextField!;
	@IBOutlet var batteryMaxCurrentValue: UITextField!;
	@IBOutlet var batteryMinVoltageValue: UITextField!;

	private func fillData()
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		setText(self.steeringMinValue, value: car.settings.steering.min);
		setText(self.steeringMaxValue, value: car.settings.steering.max);
		setText(self.steeringCenterValue, value: car.settings.steering.center);

		setText(self.throttleMinValue, value: car.settings.throttle.min);
		setText(self.throttleMaxValue, value: car.settings.throttle.max);
		setText(self.throttleCenterValue, value: car.settings.throttle.center);

		setValue(self.steeringCenterSlider, value: car.settings.steering.center);
		setValue(self.steeringMaxSlider, value: car.settings.steering.max);
		setValue(self.steeringMinSlider, value: car.settings.steering.min);

		setValue(self.throttleCenterSlider, value: car.settings.throttle.center);
		setValue(self.throttleMaxSlider, value: car.settings.throttle.max);
		setValue(self.throttleMinSlider, value: car.settings.throttle.min);

		setText(self.carNameValue, value: car.settings.name);

		setText(self.batteryVoltageValue, value: car.settings.battery.maxVoltage);
		setText(self.batteryCapacityValue, value: car.settings.battery.capacity);
		setText(self.batteryMinVoltageValue, value: car.settings.battery.minVoltage);
		setText(self.batteryMaxCurrentValue, value: car.settings.battery.maxCurrent);

		setSegment(self.mainLights, segment: Int(car.settings.lights.mainNo));
		setSegment(self.backLights, segment: Int(car.settings.lights.backNo));
		setSegment(self.reverseLights, segment: Int(car.settings.lights.reverseNo));
		setSegment(self.leftLights, segment: Int(car.settings.lights.leftNo));
		setSegment(self.rightLights, segment: Int(car.settings.lights.rightNo));

		setSegment(self.steeringAccelerometerAxis, segment: car.settings.steering.accelerometerAxis);
		setOn(self.useAccelerometer, on: car.settings.steering.useAccelerometer);
		setHidden(self.steeringAccelerometerAxisCell, hidden: !car.settings.steering.useAccelerometer);

		setOn(self.symetricalSteering, on: car.settings.steering.symmetric);
		setOn(self.invertSteering, on: car.settings.steering.inverted);
		setOn(self.invertThrottle, on: car.settings.throttle.inverted);
	}

	private func setText(field: UITextField!, value: UInt16)
	{
		setText(field, value: String(value));
	}

	private func setText(field: UITextField!, value: String)
	{
		if (nil != field)
		{
			field.text = value;
		}
	}

	override func viewWillAppear(animated: Bool)
	{
		super.viewWillAppear(animated)

		let car = DevicesManager.sharedInstance.activeCar;
		let connected = car != nil;

		setHidden(self.powerCell, hidden: !(connected && (car.settings.battery.hasVoltage || car.settings.battery.hasCurrent)));
		setHidden(self.lightsCell, hidden: !(connected && car.settings.lights.hasLights));
		setHidden(self.throttleCell, hidden: !connected);
		setHidden(self.steeringCell, hidden: !connected);
		setHidden(self.nameCell, hidden: !connected);
		setHidden(self.throttleCenterCell, hidden: !connected);

		if (connected)
		{
			if ((car.settings.type == .MiniZ) || (car.settings.type == .MiniZBLDC) || (car.settings.type == .DNano))
			{
				setHidden(self.throttleCenterCell, hidden: true);

				setMax(self.steeringCenterSlider, maximumValue: 2000);
				setMax(self.steeringMaxSlider, maximumValue: 2000);
				setMax(self.steeringMinSlider, maximumValue: 2000);

				setMin(self.throttleCenterSlider, value: 700);
				setMin(self.throttleMaxSlider, value: 700);
				setMin(self.throttleMinSlider, value: 700);
			}
			else if (car.settings.type == .Simple)
			{
				setHidden(self.throttleCenterCell, hidden: true);
				setHidden(self.steeringCenterCell, hidden: true);

				setMax(self.steeringCenterSlider, maximumValue: 700);
				setMax(self.steeringMaxSlider, maximumValue: 700);
				setMax(self.steeringMinSlider, maximumValue: 700);

				setMin(self.throttleCenterSlider, value: 700);
				setMin(self.throttleMaxSlider, value: 700);
				setMin(self.throttleMinSlider, value: 700);
			}

			fillData();
		}

		reloadDataAnimated(false);
	}

	private func setMax(slider: UISlider!, maximumValue: Int)
	{
		if (nil != slider)
		{
			slider.maximumValue = Float(maximumValue);
		}
	}

	private func setMin(slider: UISlider!, value: Int)
	{
		if (nil != slider)
		{
			slider.minimumValue = Float(value);
		}
	}

	private func setValue(slider: UISlider!, value: UInt16)
	{
		if (nil != slider)
		{
			slider.value = Float(value);
		}
	}

	private func setOn(view: UISwitch!, on: Bool)
	{
		if (nil != view)
		{
			view.on = on;
		}
	}

	private func setHidden(view: UITableViewCell!, hidden: Bool)
	{
		if (nil != view)
		{
			cell(view, setHidden: hidden);
		}
	}

	private func setSegment(view: UISegmentedControl!, segment: Int)
	{
		if (nil != view)
		{
			view.selectedSegmentIndex = segment;
		}
	}

	override func viewDidLoad()
	{
		super.viewDidLoad();

		if (nil == self.nameCell)
		{
			self.view.addGestureRecognizer(UITapGestureRecognizer(target: self, action: "dismissKeyboard"));
		}
	}

	func dismissKeyboard()
	{
		resignResponder(self.steeringMinValue);
		resignResponder(self.steeringMaxValue);
		resignResponder(self.steeringCenterValue);
		resignResponder(self.throttleMinValue);
		resignResponder(self.throttleMaxValue);
		resignResponder(self.throttleCenterValue);
		resignResponder(self.carNameValue);
		resignResponder(self.batteryVoltageValue);
		resignResponder(self.batteryCapacityValue);
		resignResponder(self.batteryMaxCurrentValue);
		resignResponder(self.batteryMinVoltageValue);
	}

	private func resignResponder(view: UIView!)
	{
		if (nil != view)
		{
			view.resignFirstResponder();
		}
	}

	@IBAction func sliderMoved(sender: UISlider)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		let value = UInt16(sender.value);

		if ((sender === self.steeringCenterSlider) || (sender === self.steeringMaxSlider) || (sender === self.steeringMinSlider))
		{
			car.steering = value;

			if (true == self.symetricalSteering.on)
			{
				if (sender === self.steeringMinSlider)
				{
					self.steeringMaxSlider.value = self.steeringCenterSlider.value + (self.steeringCenterSlider.value - self.steeringMinSlider.value);
				}
				else if (sender === self.steeringMaxSlider)
				{
					self.steeringMinSlider.value = self.steeringCenterSlider.value - (self.steeringMaxSlider.value - self.steeringCenterSlider.value);
				}
				else if (sender === self.steeringCenterSlider)
				{
					let diff = (self.steeringMaxSlider.value - self.steeringMinSlider.value) / 2.0;
					self.steeringMaxSlider.value = self.steeringCenterSlider.value + diff;
					self.steeringMinSlider.value = self.steeringCenterSlider.value - diff;
				}
			}
		}
		else if ((sender === self.throttleCenterSlider) || (sender === self.throttleMaxSlider) || (sender === self.throttleMinSlider))
		{
			car.throttle = value;
		}

		updateSlidersTexts();
	}

	@IBAction func textValueChanged(sender: UITextField)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		if (sender === self.carNameValue)
		{
			car.settings.name = sender.text!;

			return;
		}

		let value: Float! = Float(sender.text!);

		if (value == nil)
		{
			return;
		}

		if (sender === self.throttleMinValue)
		{
			self.throttleMinSlider.value = value;
		}
		else if (sender === self.throttleMaxValue)
		{
		self.throttleMaxSlider.value = value;
		}
		else if (sender === self.throttleCenterValue)
		{
		self.throttleCenterSlider.value = value;
		}
		else if (sender === self.steeringMinValue)
		{
			self.steeringMinSlider.value = value;
			sliderMoved(self.steeringMinSlider);
		}
		else if (sender === self.steeringMaxValue)
		{
			self.steeringMaxSlider.value = value;
			sliderMoved(self.steeringMaxSlider);
		}
		else if (sender === self.steeringCenterValue)
		{
			self.steeringCenterSlider.value = value;
			sliderMoved(self.steeringCenterSlider);
		}
		else if (sender === self.batteryCapacityValue)
		{
			car.settings.battery.capacity = UInt16(value);
		}
		else if (sender === self.batteryMaxCurrentValue)
		{
			car.settings.battery.maxCurrent = UInt16(value);
		}
		else if (sender === self.batteryMinVoltageValue)
		{
			car.settings.battery.minVoltage = UInt16(value);
		}
		else if (sender === self.batteryVoltageValue)
		{
			car.settings.battery.maxVoltage = UInt16(value);
		}

		updateValuesFromSlidersUI();
	}

	private func updateSliderText(textField: UITextField!, slider: UISlider!)
	{
		if ((nil != textField) && (nil != slider))
		{
			textField.text = String(Int(slider.value));
		}
	}

	private func updateSlidersTexts()
	{
		updateSliderText(self.throttleCenterValue, slider: self.throttleCenterSlider);
		updateSliderText(self.throttleMinValue, slider: self.throttleMinSlider);
		updateSliderText(self.throttleMaxValue, slider: self.throttleMaxSlider);
		updateSliderText(self.steeringCenterValue, slider: self.steeringCenterSlider);
		updateSliderText(self.steeringMinValue, slider: self.steeringMinSlider);
		updateSliderText(self.steeringMaxValue, slider: self.steeringMaxSlider);
	}

	@IBAction func sliderReleased(sender: UISlider)
	{
		let car: RadioCar! = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		updateValuesFromSlidersUI();

		if ((sender === self.steeringCenterSlider) || (sender === self.steeringMaxSlider) || (sender === self.steeringMinSlider))
		{
			car.steering = UInt16(car.settings.steering.center);
		}

		if ((sender === self.throttleCenterSlider) || (sender === self.throttleMaxSlider) || (sender === self.throttleMinSlider))
		{
			car.throttle = UInt16(car.settings.throttle.center);
		}
	}

	@IBAction func lightValueChanged(sender: UISegmentedControl)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		let selectedLightNo = UInt16(sender.selectedSegmentIndex);

		car.lights.updateLights(false, bit: 0);
		car.lights.updateLights(false, bit: 1);
		car.lights.updateLights(false, bit: 2);
		car.lights.updateLights(false, bit: 3);
		car.lights.updateLights(false, bit: 4);

		car.lights.updateLights(true, bit: selectedLightNo);

		if (sender === self.mainLights)
		{
			car.settings.lights.mainNo = selectedLightNo;
		}
		else if (sender === self.backLights)
		{
			car.settings.lights.backNo = selectedLightNo;
		}
		else if (sender === self.reverseLights)
		{
			car.settings.lights.reverseNo = selectedLightNo;
		}
		else if (sender === self.leftLights)
		{
			car.settings.lights.leftNo = selectedLightNo;
		}
		else if (sender === self.rightLights)
		{
			car.settings.lights.rightNo = selectedLightNo;
		}
	}

	@IBAction func accelerometerAxisChanged(sender: UISegmentedControl)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		car.settings.steering.accelerometerAxis = self.steeringAccelerometerAxis.selectedSegmentIndex;
	}

	@IBAction func helpSelected(sender: AnyObject)
	{
/*	UIAlertView* helpAlert = [[UIAlertView alloc] initWithTitle:@"Adjusting controlls" message:@"Throttle:\nAdjust the center value till the car is not moving.\nAdjust the min value till the car goes at max backward speed.\nAdjust the max value till the car goes at the max forward speed.\n\nSteering:\nAdjust the center value till the wheels are centered straight.\nAdjust the min value till the wheels are fully rotated to the left.\nAdjust the max value till the wheels are fully rotated to the right." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];

	[helpAlert show]; */
	}


	@IBAction func switchValueChanged(sender: UISwitch)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		let isOn = sender.on

		if (sender === self.invertSteering)
		{
			car.settings.steering.inverted = isOn;
		}
		else if (sender === self.invertThrottle)
		{
			car.settings.throttle.inverted = isOn;
		}
		else if (sender === self.symetricalSteering)
		{
			car.settings.steering.symmetric = isOn;

			if (isOn)
			{
				let diff = (self.steeringMaxSlider.value - self.steeringMinSlider.value) / 2.0;
				self.steeringMaxSlider.value = self.steeringCenterSlider.value + diff;
				self.steeringMinSlider.value = self.steeringCenterSlider.value - diff;
				updateSlidersTexts();
				updateValuesFromSlidersUI();
			}
		}
		else if (sender === self.useAccelerometer)
		{
			setHidden(self.steeringAccelerometerAxisCell, hidden: !isOn);
			reloadDataAnimated(true);
		}
	}

	private func saveSettings()
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			if (nil != self.useAccelerometer)
			{
				car.settings.steering.useAccelerometer = self.useAccelerometer.on;
			}

			if (nil != self.steeringAccelerometerAxis)
			{
				car.settings.steering.accelerometerAxis = self.steeringAccelerometerAxis.selectedSegmentIndex;
			}

			if (nil != self.carNameValue)
			{
				car.settings.name = self.carNameValue.text!;
			}

			if (nil != self.invertSteering)
			{
				car.settings.steering.inverted = self.invertSteering.on;
			}

			if (nil != self.invertThrottle)
			{
				car.settings.throttle.inverted = self.invertThrottle.on;
			}
		}
	}

	override func viewWillDisappear(animated: Bool)
	{
		saveSettings();

		super.viewWillDisappear(animated);
	}

	private func updateValuesFromSlidersUI()
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil == car)
		{
			return;
		}

		if (nil != self.steeringCenterSlider)
		{
			car.settings.steering.center = UInt16(self.steeringCenterSlider.value);
		}

		if (nil != self.steeringMaxSlider)
		{
			car.settings.steering.max = UInt16(self.steeringMaxSlider.value);
		}

		if (nil != self.steeringMinSlider)
		{
			car.settings.steering.min = UInt16(self.steeringMinSlider.value);
		}

		if (nil != self.throttleCenterSlider)
		{
			car.settings.throttle.center = UInt16(self.throttleCenterSlider.value);
		}

		if (nil != self.throttleMaxSlider)
		{
			car.settings.throttle.max = UInt16(self.throttleMaxSlider.value);
		}

		if (nil != self.throttleMinSlider)
		{
			car.settings.throttle.min = UInt16(self.throttleMinSlider.value);
		}
	}

	private func resignInput(textField: UITextField, touch: UITouch)
	{
/*	if ([textField isFirstResponder] && [touch view] != textField)
	{
	[textField resignFirstResponder];
	} */
	}

}
