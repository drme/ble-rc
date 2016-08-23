package eu.sarunas.apps.android.racecar.ble;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import android.annotation.TargetApi;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.ParcelUuid;
import eu.sarunas.apps.android.racecar.R;
import eu.sarunas.apps.android.racecar.scanner.IScanCallback;
import eu.sarunas.apps.android.racecar.scanner.Scanner;

@TargetApi(Build.VERSION_CODES.LOLLIPOP)
public class Scanner21 extends Scanner
{
	public Scanner21()
	{
		this.scanCallback = new ScanCallback()
		{
			@Override
			public void onScanResult(int callbackType, ScanResult result)
			{
				parseRecord(result.getDevice(), result.getRssi(), result.getScanRecord().getBytes());
			};

			@Override
			public void onScanFailed(int errorCode)
			{
				Scanner21.this.callBack.onError(-1);
			};

			@Override
			public void onBatchScanResults(List<ScanResult> results)
			{
				for (ScanResult result : results)
				{
					parseRecord(result.getDevice(), result.getRssi(), result.getScanRecord().getBytes());
				}
			}
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
				this.leScanner = this.bluetoothAdapter.getBluetoothLeScanner();

				if (null != this.leScanner)
				{

					ScanSettings settings = new ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build();

					ArrayList<ScanFilter> filters;

					filters = new ArrayList<ScanFilter>();

					ScanFilter filter = new ScanFilter.Builder().setServiceUuid(new ParcelUuid(UUID.fromString("a739acc0-f6cd-1692-994a-d66d9e0ce048"))).build();

					filters.add(filter);

					this.leScanner.startScan(filters, settings, this.scanCallback);
				}
			}
		}
	};

	@Override
	public void stop()
	{
		if (null != this.leScanner)
		{
			this.leScanner.stopScan(this.scanCallback);
		}

		super.stop();
	};

	private ScanCallback scanCallback = null;
	private BluetoothLeScanner leScanner = null;
	private BluetoothAdapter bluetoothAdapter = null;
};
