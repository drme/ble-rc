package eu.sarunas.apps.android.racecar;

import java.io.IOException;
import java.util.Locale;
import eu.sarunas.apps.android.racecar.ble.CompositeScanner;
import eu.sarunas.apps.android.racecar.ble.IScanCallback;
import eu.sarunas.apps.android.racecar.ble.Scanner18;
import eu.sarunas.apps.android.racecar.ble.Scanner21;
import eu.sarunas.apps.android.racecar.wifi.WiFiScanner;
import eu.sarunas.apps.android.racecar.wifi.WifiCarController;
import android.app.Activity;
import android.content.Context;
import android.os.Build;

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

	public void connect(String address, String name, Activity activity, DeviceType type, final IConnectionHandler connectionHandler) throws IOException
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
		
		if (address.equals(DemoCarController.emptyAddress))
		{
			this.connectedCar = new DemoCarController(address, name, activity, handler);
		}
		else if (address.startsWith(WifiCarController.wifiPrefix))
		{
			this.connectedCar = new WifiCarController(address.substring(WifiCarController.wifiPrefix.length()), activity, type, handler);
		}
		else
		{
			this.connectedCar = new BLECarController(address, activity, type, handler);
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
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
		{
			this.scanner = new Scanner21();
		}
		else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2)
		{
			this.scanner = new Scanner18();
		}
		else
		{
			callBack.onError(R.string.error_bluetooth_not_supported);			
			this.scanner = new Scanner();
		}
		
		this.scanner = new CompositeScanner(this.scanner, new WiFiScanner());
		
		this.scanner.start(callBack, timeOut, context);
	};
	
	private Scanner scanner = null;
	private static CarsController instance = null;
	private ICarController connectedCar = null;
};
