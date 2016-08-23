package eu.sarunas.apps.android.racecar.scanner;

import java.util.ArrayList;
import java.util.List;
import android.content.Context;
import eu.sarunas.apps.android.racecar.ble.Scanner18;
import eu.sarunas.apps.android.racecar.bluetooth.BluetoothScanner;
import eu.sarunas.apps.android.racecar.wifi.WiFiScanner;

public class CompositeScanner extends Scanner
{
	public CompositeScanner(Scanner18 leScanner, WiFiScanner wiFiScanner, BluetoothScanner scannerBluetooth)
	{
		if (null != leScanner)
		{
			this.scanners.add(leScanner);
		}
		
		this.scanners.add(wiFiScanner);
		this.scanners.add(scannerBluetooth);
	};

	@Override
	public void start(IScanCallback callBack, long timeOut, Context context)
	{
		this.callBack = callBack;
		
		this.scanners.get(0).start(new NextScanner(1, timeOut, context), timeOut, context);
	};

	@Override
	public void stop()
	{
		for (int i = this.scanners.size() - 1; i >= 0; i--)
		{
			this.scanners.get(i).stop();
		}
		
		this.callBack.onScanFinished();
	};

	class NextScanner implements IScanCallback
	{
		public NextScanner(int id, long timeOut, Context context)
		{
			this.scannerId = id;
			this.timeOut = timeOut;
			this.context = context;
		};

		@Override
		public void onCarFound(DiscoveredDevice car)
		{
			callBack.onCarFound(car);
		};

		@Override
		public void onScanStarted()
		{
			if (this.scannerId == 1)
			{
				callBack.onScanStarted();
			}
		};

		@Override
		public synchronized void onScanFinished()
		{
			if (false == nextStarted)
			{
				if (scannerId >= scanners.size())
				{
					callBack.onScanFinished();
				}
				else
				{
					scanners.get(scannerId).start(new NextScanner(scannerId + 1, timeOut, context), timeOut, context);
				}
				
				this.nextStarted = true;
			}
		};

		@Override
		public void onError(int messageId)
		{
			callBack.onError(messageId);
			onScanFinished();
		};
		
		private boolean nextStarted = false;
		private int scannerId;
		private long timeOut;
		private Context context;
	};
	
	private IScanCallback callBack = null;
	private List<Scanner> scanners = new ArrayList<Scanner>();
};
