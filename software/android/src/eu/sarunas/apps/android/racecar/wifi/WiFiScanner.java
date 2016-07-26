package eu.sarunas.apps.android.racecar.wifi;

import java.util.HashSet;
import java.util.Locale;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.WifiManager;
import android.widget.Toast;
import eu.sarunas.apps.android.racecar.DeviceType;
import eu.sarunas.apps.android.racecar.Scanner;
import eu.sarunas.apps.android.racecar.ble.IScanCallback;

public class WiFiScanner extends Scanner
{
	public WiFiScanner()
	{
		this.scanReceiver = new BroadcastReceiver()
		{
			@Override
			public void onReceive(Context c, Intent intent)
			{
				for (android.net.wifi.ScanResult result : WiFiScanner.this.wifi.getScanResults())
				{
					if (true == result.SSID.startsWith(WifiCarController.wifiPrefix))
					{
						if (false == WiFiScanner.this.discoveredCars.contains(result.BSSID.toUpperCase(Locale.FRENCH)))
						{
							WiFiScanner.this.discoveredCars.add(result.BSSID.toUpperCase(Locale.FRENCH));

							WiFiScanner.this.callBack.onCarFound(new WifiCarController(result, DeviceType.RC));
						}
					}
				}
			}
		};
	};

	@Override
	public void start(final IScanCallback callBack, long timeOut, Context context)
	{
		this.discoveredCars.clear();

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

	private HashSet<String> discoveredCars = new HashSet<String>();
	private WifiManager wifi = null;
	private BroadcastReceiver scanReceiver;
	private Context context = null;
};
