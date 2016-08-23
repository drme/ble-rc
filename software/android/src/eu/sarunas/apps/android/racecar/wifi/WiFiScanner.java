package eu.sarunas.apps.android.racecar.wifi;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.widget.Toast;
import eu.sarunas.apps.android.racecar.controller.DeviceType;
import eu.sarunas.apps.android.racecar.scanner.DiscoveredDevice;
import eu.sarunas.apps.android.racecar.scanner.IScanCallback;
import eu.sarunas.apps.android.racecar.scanner.Scanner;

public class WiFiScanner extends Scanner
{
	public WiFiScanner()
	{
		this.scanReceiver = new BroadcastReceiver()
		{
			@Override
			public void onReceive(Context c, Intent intent)
			{
				for (ScanResult result : WiFiScanner.this.wifi.getScanResults())
				{
					if (true == result.SSID.startsWith(WifiCarController.wifiPrefix))
					{
						WiFiScanner.this.callBack.onCarFound(new DiscoveredDevice(WifiCarController.wifiPrefix + result.BSSID + "|" + result.SSID, result.SSID.substring(WifiCarController.wifiPrefix.length()).replace('_', ' '), DeviceType.RC));
					}
				}
			}
		};
	};

	@Override
	public void start(final IScanCallback callBack, long timeOut, Context context)
	{
		if (true == this.stopped)
		{
			return;
		}
		
		super.start(callBack, timeOut, context);

		this.wifi = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);

		if (this.wifi.isWifiEnabled() == false)
		{
			Toast.makeText(context, "wifi is disabled..making it enabled", Toast.LENGTH_LONG).show();

			this.wifi.setWifiEnabled(true);
		}

		context.registerReceiver(this.scanReceiver, new IntentFilter(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION));

		this.context = context;

		this.wifi.startScan();
	};

	@Override
	public void stop()
	{
		this.stopped = true;
		
		if (null != this.wifi)
		{
		}

		if (null != this.context)
		{
			this.context.unregisterReceiver(this.scanReceiver);
			this.context = null;
		}

		super.stop();
	};

	private boolean stopped = false;
	private WifiManager wifi = null;
	private BroadcastReceiver scanReceiver;
	private Context context = null;
};
