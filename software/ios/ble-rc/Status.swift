import Foundation;
import CoreBluetooth;

enum StatusType
{
	case Disabled;
	case Minimal;
	case Full;
}

class Status
{
	let battery: BatteryStatus;
	let temperature: TemperatureStatus;
	let acceleration: AccelerationStatus;
	var refreshInterval = 0.1;
	var updated: Event!;
	private(set) var latency: Float = 0.0;	
	private var latencyMeasureStart: NSDate!;
	private var reads = 0;

	init(inout characteristics: [CharacteristicProxy])
	{
		self.temperature = TemperatureStatus(characteristics: &characteristics);
		self.battery = BatteryStatus(characteristics: &characteristics);
		self.acceleration = AccelerationStatus(characteristics: &characteristics);

		self.battery.controllerBatteryVoltageProxy.updated = self.measureLatency;
	}

	func getRequiredIds(long: Bool, type: RadioCarType) -> [CBUUID]
	{
		var ids: [CBUUID] = [];

		ids += self.battery.getRequiredIds(long, type: type);
		ids += self.temperature.getRequiredIds(long, type: type);

		return ids;
	}

	private func refresh()
	{
		if (self.type != .Disabled)
		{
			self.latencyMeasureStart = NSDate();

			self.reads = self.battery.refresh(self.type == .Minimal);

			if (self.type == .Full)
			{
				self.reads += self.temperature.refresh();
				self.reads += self.acceleration.refresh();
			}
		}
	}

	private func measureLatency()
	{
		if (nil != self.latencyMeasureStart)
		{
			let packets = 1.0 + Double(self.reads);

			self.latency = Float(self.latencyMeasureStart.timeIntervalSinceNow / -packets);

			NSLog("Latency: " + String(self.latency));
		}

		dispatch_after(dispatch_time(DISPATCH_TIME_NOW, Int64(self.refreshInterval * Double(NSEC_PER_SEC))), dispatch_get_main_queue())
		{
			self.refresh();
		}

		if (nil != self.updated)
		{
			self.updated();
		}
	}

	var type: StatusType = .Disabled
	{
		didSet
		{
			if (self.type != .Disabled)
			{
				refresh();
			}
		}
	}
}
