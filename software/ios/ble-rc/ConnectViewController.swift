import Foundation
import UIKit

class ConnectViewControllerEx : UIViewController, UITableViewDelegate, UITabBarControllerDelegate, UITableViewDataSource
{
	@IBOutlet var radioCarsTableView: UITableView!;
	private var blueToothEnabled = false;
	private var refreshControl: UIRefreshControl!;
	private var connectDialog: UIAlertController!;

	@IBAction func addClicked(sender: AnyObject!)
	{
		let optionMenu = UIAlertController(title: "Add car", message: AppSettings.addMessage, preferredStyle: .Alert);

		optionMenu.addAction(UIAlertAction(title: "Get some cars", style: .Default, handler: { _ -> Void in UIApplication.sharedApplication().openURL(NSURL(string: AppSettings.shopForCarsUrl)!); }));

		optionMenu.addAction(UIAlertAction(title: "Add fake demo cars", style: .Default, handler: { _ -> Void in DevicesManager.sharedInstance.addFakes(); }));
		optionMenu.addAction(UIAlertAction(title: "Remove fake demo cars", style: .Default, handler: { _ -> Void in DevicesManager.sharedInstance.removeFakes(); }));
		optionMenu.addAction(UIAlertAction(title: "Cancel", style: .Cancel, handler: nil));

		self.presentViewController(optionMenu, animated: true, completion: nil);
	}

	func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int
	{
		if (true == self.blueToothEnabled)
		{
			return DevicesManager.sharedInstance.radioCars.count;
		}
		else
		{
			return 0;
		}
	}
	
	func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell
	{
		var cell: UITableViewCell! = tableView.dequeueReusableCellWithIdentifier("ItemCell");

		if (nil == cell)
		{
			cell = UITableViewCell(style: .Subtitle, reuseIdentifier: "ItemCell")
		}

		let carItem = DevicesManager.sharedInstance.radioCars[indexPath.row];

		cell.textLabel?.text = carItem.settings.name;

		if (carItem === DevicesManager.sharedInstance.activeCar)
		{
			cell.detailTextLabel?.text = "Already connected";
		}
		else
		{
			if (carItem.comment.characters.count > 0)
			{
				cell.detailTextLabel?.text = carItem.comment;
			}
			else
			{
				cell.detailTextLabel?.text = carItem.settings.type.description;
			}
		}

		cell.selectionStyle = .None;
		cell.accessoryType = .DisclosureIndicator;
		
		return cell;
	}

	override func viewDidLoad()
	{
		super.viewDidLoad();

		self.tabBarController?.delegate = self;

		DevicesManager.sharedInstance.searchEnded = self.searchEnded;
		DevicesManager.sharedInstance.bluetoothTurnedOff = self.bluetoothTurnedOff;
		DevicesManager.sharedInstance.bluetoothTurnedOn = self.bluetoothTurnedOn;
		DevicesManager.sharedInstance.deviceFound = self.reloadTable;
		DevicesManager.sharedInstance.connected = self.connected;
		DevicesManager.sharedInstance.disconnected = self.disconnected;

		self.refreshControl = UIRefreshControl();
		self.refreshControl.backgroundColor = UIColor(white: 230.0 / 255.0, alpha: 1.0);
		self.refreshControl.addTarget(self, action: "searchClicked", forControlEvents: .ValueChanged);

		let tableViewController = UITableViewController();
		tableViewController.tableView = self.radioCarsTableView;
		tableViewController.refreshControl = self.refreshControl;
	}

	func searchClicked()
	{
		DevicesManager.sharedInstance.startDeviceSearch();
	}

	private func reloadTable()
	{
		dispatch_async(dispatch_get_main_queue(),
		{
			self.radioCarsTableView?.reloadData()
		});
	}

	private func bluetoothTurnedOn()
	{
		self.blueToothEnabled = true;
		self.refreshControl.enabled = true;

		self.radioCarsTableView.backgroundView = nil;
		self.radioCarsTableView.separatorStyle = .SingleLine;
		self.refreshControl.alpha = 1;

		searchClicked();
	}

	private func bluetoothTurnedOff()
	{
		self.blueToothEnabled = false;
		self.refreshControl.enabled = false;

		DevicesManager.sharedInstance.disconnectActive();

		self.tabBarController!.selectedIndex = 0;

		self.radioCarsTableView.backgroundView = UILabel(frame: self.radioCarsTableView.bounds);
		(self.radioCarsTableView.backgroundView as! UILabel).text = "This app requires Bluetooth\nPlease enable Bluetooth.";
		(self.radioCarsTableView.backgroundView as! UILabel).numberOfLines = 0;
		(self.radioCarsTableView.backgroundView as! UILabel).textAlignment = .Center;
		self.radioCarsTableView.separatorStyle = .None;
		self.refreshControl.alpha = 0;

		reloadTable();
	}

	private func searchEnded()
	{
		if ((DevicesManager.sharedInstance.radioCars.count <= 0) && (true == self.blueToothEnabled))
		{
			let fakeCar = RadioCar(name: "Virtual Demo Car", comment: "No cars found. Please pull down to refresh.");

			DevicesManager.sharedInstance.radioCars.append(fakeCar);

			reloadTable();
		}

		if (nil != self.refreshControl)
		{
			self.refreshControl.endRefreshing();
		}
	}

	private func connected()
	{
		if (nil != self.connectDialog)
		{
			self.connectDialog.dismissViewControllerAnimated(true, completion: {});
			self.connectDialog = nil;
		}

		self.tabBarController!.selectedIndex = 1;
	}

	private func disconnected()
	{
		if (nil != self.connectDialog)
		{
			self.connectDialog.dismissViewControllerAnimated(true, completion: {});
			self.connectDialog = nil;
		}

		self.tabBarController!.selectedIndex = 0;
	}

	func tabBarController(tabBarController: UITabBarController, shouldSelectViewController viewController: UIViewController) -> Bool
	{
		let connected = (nil != DevicesManager.sharedInstance.activeCar);

		if (viewController.tabBarItem.tag == 22)
		{
			return true;
		}
		else if (viewController.tabBarItem.tag == 11)
		{
			return connected;
		}

		return true;
	}

	func tabBarController(tabBarController: UITabBarController, didSelectViewController viewController: UIViewController)
	{
		if (viewController.tabBarItem.tag == 22)
		{
			searchClicked();
		}
	}

	private func connect(selectedIndexPath: NSIndexPath!)
	{
		if (nil != selectedIndexPath)
		{
			if ((selectedIndexPath.row >= 0) && (selectedIndexPath.row < DevicesManager.sharedInstance.radioCars.count))
			{
				let carItem = DevicesManager.sharedInstance.radioCars[selectedIndexPath.row];

				if (carItem === DevicesManager.sharedInstance.activeCar)
				{
					self.tabBarController!.selectedIndex = 1;
					return;
				}

				if (nil == self.connectDialog)
				{
					self.connectDialog = UIAlertController(title: nil, message: "Connecting to " + carItem.settings.name + "...\n\n\n", preferredStyle: .Alert)

					let indicator = UIActivityIndicatorView(frame: self.connectDialog.view.bounds)
					indicator.autoresizingMask = [ UIViewAutoresizing.FlexibleWidth, UIViewAutoresizing.FlexibleHeight ];
					indicator.tintColor = AppSettings.applicationTint;
					self.connectDialog.view.addSubview(indicator);
					indicator.userInteractionEnabled = false;
					indicator.startAnimating();

					self.connectDialog.addAction(UIAlertAction(title: "Cancel", style: .Cancel, handler: { _ in DevicesManager.sharedInstance.cancelConnection(carItem); }));
					self.presentViewController(self.connectDialog, animated: true, completion: nil)
				}

				DevicesManager.sharedInstance.connect(carItem);
			}
		}
	}

	func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath)
	{
		connect(indexPath);
	}
}
