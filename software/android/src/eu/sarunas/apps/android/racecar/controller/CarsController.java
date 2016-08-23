package eu.sarunas.apps.android.racecar.controller;

import java.io.IOException;
import java.util.Locale;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.os.Build;
import eu.sarunas.apps.android.racecar.R;
import eu.sarunas.apps.android.racecar.ble.BLECarController;
import eu.sarunas.apps.android.racecar.ble.Scanner18;
import eu.sarunas.apps.android.racecar.bluetooth.BluetoothController;
import eu.sarunas.apps.android.racecar.bluetooth.BluetoothScanner;
import eu.sarunas.apps.android.racecar.demo.DemoCarController;
import eu.sarunas.apps.android.racecar.scanner.CompositeScanner;
import eu.sarunas.apps.android.racecar.scanner.IScanCallback;
import eu.sarunas.apps.android.racecar.scanner.Scanner;
import eu.sarunas.apps.android.racecar.wifi.WiFiScanner;
import eu.sarunas.apps.android.racecar.wifi.WifiCarController;

public class CarsController
{
	public ICarController getConnectedCar()
	{
		return this.connectedCar;
	};

	public static CarsController getInstance()
	{
		if (null == CarsController.instance)
		{
			CarsController.instance = new CarsController();
		}

		return CarsController.instance;
	};
	
	public void wifiConnected(String bsid)
	{
		if (this.connectedCar instanceof WifiCarController)
		{
			WifiCarController car = (WifiCarController) this.connectedCar;
			
			if ((null != bsid) && (car.getAddress().toUpperCase(Locale.FRANCE).equals(bsid.toUpperCase(Locale.FRANCE))))
			{
				car.connectToServer();
			}
		}
	};
	
	public void wifiDisconnected(String bsid)
	{
		if (this.connectedCar instanceof WifiCarController)
		{
			WifiCarController car = (WifiCarController) this.connectedCar;
			
			if ((null != bsid) && (car.getAddress().toUpperCase(Locale.FRANCE).equals(bsid.toUpperCase(Locale.FRANCE))))
			{
				car.disconnect();
			}
		}
	};

	@SuppressLint("DefaultLocale")
	public void connect(String address, String name, Activity activity, final IConnectionHandler connectionHandler) throws IOException
	{
		IConnectionHandler handler = new IConnectionHandler()
		{
			@Override
			public void onDisconnected(Object client, String message)
			{
				CarsController.this.connectedCar = null;
				connectionHandler.onDisconnected(client, message);
			};

			@Override
			public void onConnected(ICarController car)
			{
				CarsController.this.connectedCar = car;
				connectionHandler.onConnected(car);
			};

			@Override
			public void onStatsChanged(ICarController car)
			{
				connectionHandler.onStatsChanged(car);
			};
		};
		
		if (address.length() == 1)
		{
			this.connectedCar = new DemoCarController(address, name, activity, handler);
		}
		else if (address.startsWith(WifiCarController.wifiPrefix))
		{
			int length = WifiCarController.wifiPrefix.length();
			String bssid = address.substring(length, length + 17).toUpperCase();
			String ssid = address.substring(length + 17 + 1);
			
			this.connectedCar = new WifiCarController(bssid, ssid, activity, handler);
		}
		else if (address.startsWith(BluetoothController.btPrefix))
		{
			this.connectedCar = new BluetoothController(address.substring(BluetoothController.btPrefix.length()), handler);
		}
		else
		{
			this.connectedCar = new BLECarController(address, activity, DeviceType.RC, handler);
		}
	};

	public void stopScan()
	{
		if (null != this.scanner)
		{
			this.scanner.stop();
		}
	};
	
	public void scanDevices(IScanCallback callBack, long timeOut, Context context)
	{
		Scanner18 leScanner = null;
		
		/*if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
		{
			this.scanner = new Scanner21();
		}
		else*/ if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2)
		{
			leScanner = new Scanner18();
		}
		else
		{
			callBack.onError(R.string.error_bluetooth_not_supported);			
			leScanner = null;
		}
		
		this.scanner = new CompositeScanner(leScanner, new WiFiScanner(), new BluetoothScanner());
		
		this.scanner.start(callBack, timeOut, context);
	};
	
	private Scanner scanner = null;
	private static CarsController instance = null;
	private ICarController connectedCar = null;
};
