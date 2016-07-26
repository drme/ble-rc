import UIKit

class FirmwareUpdateViewController: UIViewController
{
	@IBOutlet var firmwareInfoButton: UIButton!;
	@IBOutlet var firmwareUpdateButton: UIButton!;
	@IBOutlet var cancelButton: UIButton!;
	@IBOutlet weak var firmwareVersionLabel: UILabel!;
	@IBOutlet weak var hardwareVersionLabel: UILabel!;
	@IBOutlet weak var imageVersionLabel: UILabel!;
	@IBOutlet weak var carTypeLabel: UILabel!;
	@IBOutlet weak var firmwareUpdateProgressView: UIView!;
	@IBOutlet weak var firmwareUpdateProgressBar: UIProgressView!;
	@IBOutlet weak var firmwareUpdateLabel1: UILabel!;
	@IBOutlet weak var firmwareUpdateLabel2: UILabel!;

	override func viewDidLoad()
	{
		super.viewDidLoad();

		self.firmwareInfoButton.backgroundColor = AppSettings.applicationTint;
		self.firmwareUpdateButton.backgroundColor = AppSettings.applicationTint;
		self.cancelButton.backgroundColor = AppSettings.applicationTint;
	}

	@IBAction func updateFirmwareSelected(sender: UIButton)
	{
		startUpdate(sender);
	}

	func startUpdate(sender: UIButton)
	{
		let actionSheetController: UIAlertController = UIAlertController(title: nil, message: "Select Firmware image", preferredStyle: .ActionSheet);

		actionSheetController.addAction(UIAlertAction(title: "Cancel", style: .Cancel) { action -> Void in });

		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			let updater = car.firmwareUpdater;

			for file in updater.filterFirmwareFiles(self.isValidFirmwre)
			{
				let action: UIAlertAction = UIAlertAction(title: file.displayName, style: .Default) { action -> Void in self.startUpdate(file) };

				actionSheetController.addAction(action);
			}

			actionSheetController.popoverPresentationController?.sourceView = sender as UIView;
			actionSheetController.view.tintColor = AppSettings.applicationTint;

			self.presentViewController(actionSheetController, animated: true, completion: nil);
		}
	}

	private func isValidFirmwre(file: FirmwareFile) -> Bool
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			switch(car.settings.type)
			{
				case .MiniZBLDC:
					return file.fileName.containsString("mini-z-bldc");
				default:
					return false;
			}
		}

		return false;
	}

	private func startUpdate(firmwareFile : FirmwareFile)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			car.firmwareUpdater.startUpdate(firmwareFile, failedHandler: self.onUpdateFailed, succeededHandler: self.onUpdateSucceeded, disconnectedHandler: self.onUpdateFailed, badImageHandler: self.onWrongFirmwareFile, progressHandler: self.onStatusUpdated);
		}
	}

	private func onUpdateFailed()
	{
		self.firmwareUpdateProgressView.hidden = true

		self.navigationController?.popToRootViewControllerAnimated(true);

		showMessage("FW Upgrade Failed", message: "Device disconnected during programming, firmware upgrade was not finished");
	}

	private func onUpdateSucceeded()
	{
		CATransaction.begin()
		CATransaction.setCompletionBlock({ self.showMessage("Firmware upgrade complete", message: "Firmware upgrade was successfully completed, device needs to be restarted (please remove and reinsert the battery)") })

		self.firmwareUpdateProgressView.hidden = true;

//		self.navigationController!.popToRootViewControllerAnimated(true)

		CATransaction.commit()

//		DevicesManager.sharedInstance.disconnectActive();
	}

	private func onWrongFirmwareFile(type: UInt8)
	{
		showMessage("", message: "Firmware that was selected was of type: " + (type > 0 ? "B" : "A") + ", which is the same as on the device, please select another firmware");
	}

	private func onStatusUpdated(percent: Float, secondsLeft: Float)
	{
		let minutesLeft = Int(secondsLeft / 60);
		let secondsRemainderLeft = Int(Int(secondsLeft) - minutesLeft * 60);

		self.firmwareUpdateProgressView.hidden = false;
		self.firmwareUpdateProgressBar.progress = percent;
		self.firmwareUpdateLabel1.text = "" + String(Int(percent * 100.0)) + "." + String(Int(percent % 1000.0) % 100) + "%";
		self.firmwareUpdateLabel2.text = "Time remaining: " + String(minutesLeft) + ":" + (secondsRemainderLeft < 10 ? "0" : "")  + String(secondsRemainderLeft);
	}

	private func showMessage(title: String, message: String)
	{
		let alert = UIAlertController(title: title, message: message, preferredStyle: UIAlertControllerStyle.Alert);
		alert.addAction(UIAlertAction(title: "OK", style: UIAlertActionStyle.Default, handler: nil));

		UIApplication.sharedApplication().delegate?.window?!.rootViewController?.presentViewController(alert, animated: true, completion: nil);
	}

	@IBAction func cancelFirmwareUpdateSelected(sender: UIButton!)
	{
		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			car.firmwareUpdater.cancelUpdate();
		}

		self.firmwareUpdateProgressView.hidden = true;
	}

	override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?)
	{
		let controller = segue.destinationViewController as! FirmwareInformationViewController

		controller.logFile = AppSettings.shopForCarsUrl;
	}

	override func viewWillDisappear(animated: Bool)
	{
		cancelFirmwareUpdateSelected(nil);

		UIApplication.sharedApplication().idleTimerDisabled = false;

		super.viewWillDisappear(animated)
	}

	override func viewWillAppear(animated: Bool)
	{
		UIApplication.sharedApplication().idleTimerDisabled = true;

		let car = DevicesManager.sharedInstance.activeCar;

		if (nil != car)
		{
			self.firmwareVersionLabel.text = car.firmwareUpdater.firmwareVersion;
			self.hardwareVersionLabel.text = car.firmwareUpdater.hardwareVersion;
			self.imageVersionLabel.text = (car.firmwareUpdater.currentImageVersion & 0x01 > 0) ? "B" : "A";
			self.carTypeLabel.text = String(car.settings.type);
			self.firmwareUpdateButton.backgroundColor = self.firmwareInfoButton.backgroundColor;
			self.firmwareUpdateButton.enabled = true;
		}
		else
		{
			self.firmwareVersionLabel.text = "";
			self.hardwareVersionLabel.text = "";
			self.imageVersionLabel.text = "";
			self.carTypeLabel.text = "";
			self.firmwareUpdateButton.backgroundColor = UIColor.lightGrayColor();
			self.firmwareUpdateButton.enabled = false;
		}

		self.firmwareUpdateProgressView.hidden = true

		super.viewWillAppear(animated);
	}
}
