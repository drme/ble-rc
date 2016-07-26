package eu.sarunas.apps.android.racecar.ble;

import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothAdapter.LeScanCallback;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import eu.sarunas.apps.android.racecar.R;
import eu.sarunas.apps.android.racecar.Scanner;

@SuppressWarnings("deprecation")
@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
public class Scanner18 extends Scanner
{
	public Scanner18()
	{
		this.leScanCallback = new LeScanCallback()
		{
			@Override
			public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord)
			{
				parseRecord(device, rssi, scanRecord);
			};
		};
	};

	@Override
	public void start(IScanCallback callBack, long timeOut, Context context)
	{
		super.start(callBack, timeOut, context);

		if (false == context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE))
		{
			this.callBack.onError(R.string.ble_not_supported);
		}
		else
		{
			BluetoothManager bluetoothManager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);

			if (bluetoothManager != null)
			{
				this.bluetoothAdapter = bluetoothManager.getAdapter();
			}

			if (null == this.bluetoothAdapter)
			{
				this.callBack.onError(R.string.error_bluetooth_not_supported);
			}
			else
			{
				this.bluetoothAdapter.startLeScan(this.leScanCallback);
			}
		}
	};

	@Override
	public void stop()
	{
		if (null != this.bluetoothAdapter)
		{
			this.bluetoothAdapter.stopLeScan(this.leScanCallback);
		}

		super.stop();
	};

	private LeScanCallback leScanCallback = null;
	private BluetoothAdapter bluetoothAdapter = null;
};
