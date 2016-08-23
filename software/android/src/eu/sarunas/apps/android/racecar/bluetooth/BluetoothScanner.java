package eu.sarunas.apps.android.racecar.bluetooth;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import eu.sarunas.apps.android.racecar.controller.DeviceType;
import eu.sarunas.apps.android.racecar.scanner.DiscoveredDevice;
import eu.sarunas.apps.android.racecar.scanner.IScanCallback;
import eu.sarunas.apps.android.racecar.scanner.Scanner;

public class BluetoothScanner extends Scanner
{
	public BluetoothScanner()
	{
		this.scanReceiver = new BroadcastReceiver()
		{
			@SuppressLint("NewApi")
			@Override
			public void onReceive(Context c, Intent intent)
			{
				String action = intent.getAction();

				if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action))
				{
				}
				else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action))
				{
				}
				else if (BluetoothDevice.ACTION_FOUND.equals(action))
				{
					BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

					try
					{
						if (device.getType() == BluetoothDevice.DEVICE_TYPE_CLASSIC)
						{
							BluetoothScanner.this.callBack.onCarFound(new DiscoveredDevice(BluetoothController.btPrefix + device.getAddress(), device.getName(), DeviceType.Simple));
						}
					}
					catch (Throwable ex)
					{
						BluetoothScanner.this.callBack.onCarFound(new DiscoveredDevice(BluetoothController.btPrefix + device.getAddress(), device.getName(), DeviceType.Simple));
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

		this.context = context;

		BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();

		IntentFilter filter = new IntentFilter();

		filter.addAction(BluetoothDevice.ACTION_FOUND);
		filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
		filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);

		context.registerReceiver(this.scanReceiver, filter);

		adapter.startDiscovery();
	};

	@Override
	public void stop()
	{
		this.stopped = true;

		if (null != this.context)
		{
			this.context.unregisterReceiver(this.scanReceiver);
			this.context = null;
		}

		super.stop();
	};

	private boolean stopped = false;
	private BroadcastReceiver scanReceiver;
	private Context context = null;
};
