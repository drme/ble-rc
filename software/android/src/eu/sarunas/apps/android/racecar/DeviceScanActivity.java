package eu.sarunas.apps.android.racecar;

import android.annotation.TargetApi;
import android.app.Activity;
import android.app.ListActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.NavUtils;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ListView;
import android.widget.Toast;
import eu.sarunas.apps.android.racecar.controller.CarsController;
import eu.sarunas.apps.android.racecar.scanner.DiscoveredDevice;
import eu.sarunas.apps.android.racecar.scanner.IScanCallback;
import eu.sarunas.apps.android.racecar.utils.AppSettings;
import eu.sarunas.apps.android.racecar.utils.DeviceListAdapter;

public class DeviceScanActivity extends ListActivity implements IScanCallback
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		AppSettings.getInstance().updateTitle(this, true);
	};

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.menu_devices_scan, menu);

		if (false == this.scanning)
		{
			menu.findItem(R.id.menu_stop).setVisible(false);
			menu.findItem(R.id.menu_scan).setVisible(true);
			menu.findItem(R.id.menu_refresh).setActionView(null);
		}
		else
		{
			menu.findItem(R.id.menu_stop).setVisible(true);
			menu.findItem(R.id.menu_scan).setVisible(false);
			menu.findItem(R.id.menu_refresh).setActionView(R.layout.actionbar_indeterminate_progress);
		}

		return true;
	};

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
			case android.R.id.home:
				NavUtils.navigateUpFromSameTask(this);
				overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
				break;
			case R.id.menu_scan:
				this.deviceListAdapter.clear();
				CarsController.getInstance().scanDevices(this, DeviceScanActivity.SCAN_PERIOD, this);
				break;
			case R.id.menu_stop:
				CarsController.getInstance().stopScan();
				break;
			case R.id.menu_get_cars:
				String url = (Target.current == AppType.SmartRacer) ? "https://www.tindie.com/products/some1/smart-racer/" : "https://www.tindie.com/products/SingletonLabs/ble-for-mini-z/";
				Intent browserIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
				startActivity(browserIntent);
				overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
				break;
			case R.id.menu_add_fakes:
				this.deviceListAdapter.addFakes();
				break;
			case R.id.menu_remove_fakes:
				this.deviceListAdapter.removeFakes();
				break;
		}

		return true;
	};

	@Override
	protected void onResume()
	{
		super.onResume();

		requestBluetoothEnable();

		this.deviceListAdapter = new DeviceListAdapter(getLayoutInflater());
		setListAdapter(this.deviceListAdapter);

		CarsController.getInstance().scanDevices(this, DeviceScanActivity.SCAN_PERIOD, this);
	};

	@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR2)
	private void requestBluetoothEnable()
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2)
		{
			BluetoothManager bluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);

			if (bluetoothManager != null)
			{
				BluetoothAdapter bluetoothAdapter = bluetoothManager.getAdapter();

				if (null != bluetoothAdapter)
				{
					if (false == bluetoothAdapter.isEnabled())
					{
						Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
						startActivityForResult(enableBtIntent, DeviceScanActivity.REQUEST_ENABLE_BT);
					}
				}
			}
		}
	};

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if ((requestCode == DeviceScanActivity.REQUEST_ENABLE_BT) && (resultCode == Activity.RESULT_CANCELED))
		{
			finish();
			overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
			return;
		}

		super.onActivityResult(requestCode, resultCode, data);
	};

	@Override
	protected void onPause()
	{
		super.onPause();

		CarsController.getInstance().stopScan();

		this.deviceListAdapter.clear();
	};

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id)
	{
		DiscoveredDevice car = this.deviceListAdapter.getDevice(position);

		if (null == car)
		{
			return;
		}

		CarsController.getInstance().stopScan();

		Bundle data = new Bundle();
		data.putString(BluetoothDevice.EXTRA_DEVICE, car.getAddress());
		data.putString(BluetoothDevice.EXTRA_NAME, car.getName());
		
		Intent result = new Intent();
		result.putExtras(data);

		setResult(Activity.RESULT_OK, result);

		finish();
		overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
	};

	@Override
	public void onDestroy()
	{
		CarsController.getInstance().stopScan();
		super.onDestroy();
	};

	@Override
	public void onBackPressed()
	{
		super.onBackPressed();
		overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
	};

	@Override
	public void onCarFound(final DiscoveredDevice car)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				DeviceScanActivity.this.deviceListAdapter.addDevice(car);
			}
		});
	};

	@Override
	public void onScanStarted()
	{
		this.scanning = true;
		updateMenus();
	};

	@Override
	public void onScanFinished()
	{
		this.scanning = false;
		updateMenus();
	};

	@Override
	public void onError(final int messageId)
	{
		if (messageId != -1)
		{
			runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					Toast.makeText(DeviceScanActivity.this, messageId, Toast.LENGTH_SHORT).show();
				};
			});
		}
	};

	private void updateMenus()
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				invalidateOptionsMenu();
			};
		});
	};
	
	private DeviceListAdapter deviceListAdapter = null;
	private boolean scanning = false;
	private static final int REQUEST_ENABLE_BT = 1;
	private static final long SCAN_PERIOD = 10000;
};
