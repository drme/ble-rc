package eu.sarunas.apps.android.racecar;

import java.io.IOException;
import java.lang.reflect.Field;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.DetailedState;
import android.net.wifi.SupplicantState;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewConfiguration;
import android.widget.Toast;
import eu.sarunas.apps.android.racecar.controller.CarsController;
import eu.sarunas.apps.android.racecar.controller.ICarController;
import eu.sarunas.apps.android.racecar.controller.IConnectionHandler;
import eu.sarunas.apps.android.racecar.utils.AppSettings;
import eu.sarunas.apps.android.racecar.utils.Light;
import eu.sarunas.apps.android.racecar.utils.ThumbStickView;
import eu.sarunas.apps.android.racecar.utils.ThumbStickView.IValueChanged;

public class MainActivity extends Activity implements SensorEventListener
{
	public MainActivity()
	{
		super();
		
		this.wifiStateReceiver = new BroadcastReceiver()
		{
			public void onReceive(Context context, Intent intent)
			{
				if (isInitialStickyBroadcast())
				{
					return;
				}
				
				String action = intent.getAction();

				if (action.equals(WifiManager.WIFI_STATE_CHANGED_ACTION))
				{
					checkState(intent.getIntExtra(WifiManager.EXTRA_WIFI_STATE, WifiManager.WIFI_STATE_UNKNOWN));
				}
				else if (action.equals(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION))
				{
					changeState(WifiInfo.getDetailedStateOf((SupplicantState) intent.getParcelableExtra(WifiManager.EXTRA_SUPPLICANT_CONNECTED)));
				}
				else if (action.equals(WifiManager.NETWORK_STATE_CHANGED_ACTION))
				{
					changeState(((NetworkInfo) intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO)).getDetailedState());
				}
			};
			
			private synchronized void changeState(DetailedState state)
			{
				if (state == DetailedState.SCANNING)
				{
					Log.d("wifiSupplicanState", "SCANNING");
				}
				else if (state == DetailedState.CONNECTING)
				{
					Log.d("wifiSupplicanState", "CONNECTING");
				}
				else if (state == DetailedState.OBTAINING_IPADDR)
				{
					Log.d("wifiSupplicanState", "OBTAINING_IPADDR");
				}
				else if (state == DetailedState.CONNECTED)
				{
					Log.d("wifiSupplicanState", "CONNECTED");
					
					WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);

					WifiInfo wifiInfo = wifiManager.getConnectionInfo();
					
					this.conenctedWifi = wifiInfo.getBSSID();

					CarsController.getInstance().wifiConnected(this.conenctedWifi);
				}
				else if (state == DetailedState.DISCONNECTING)
				{
					Log.d("wifiSupplicanState", "DISCONNECTING");
				}
				else if (state == DetailedState.DISCONNECTED)
				{
					Log.d("wifiSupplicanState", "DISCONNECTTED");

					CarsController.getInstance().wifiDisconnected(this.conenctedWifi);
					
					this.conenctedWifi = null;
				}
				else if (state == DetailedState.FAILED)
				{
				}
			};

			public void checkState(int state)
			{
				if (state == WifiManager.WIFI_STATE_ENABLING)
				{
					Log.d("WifiManager", "WIFI_STATE_ENABLING");
				}
				else if (state == WifiManager.WIFI_STATE_ENABLED)
				{
					Log.d("WifiManager", "WIFI_STATE_ENABLED");
				}
				else if (state == WifiManager.WIFI_STATE_DISABLING)
				{
					Log.d("WifiManager", "WIFI_STATE_DISABLING");
				}
				else if (state == WifiManager.WIFI_STATE_DISABLED)
				{
					Log.d("WifiManager", "WIFI_STATE_DISABLED");
				}
			};
			
			private String conenctedWifi = null;
		};
	};

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		AppSettings.getInstance().updateTitle(this, false);
		AppSettings.getInstance().setActivity(this);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		getOverflowMenu();
		
		this.thumbStickThrottle = (ThumbStickView) findViewById(R.id.thumbStickViewThrottle);
		this.thumbStickThrottle.setEnabled(false);
		this.thumbStickThrottle.setArrowImage((Target.current == AppType.BleZee) ? R.drawable.throttle_arrow_b : R.drawable.throttle_arrow);
		this.thumbStickThrottle.setVertical(false);
		this.thumbStickThrottle.setValueChangedHandler(new IValueChanged()
		{
			@Override
			public void onValueChanged(float x, float y)
			{
				if (null != CarsController.getInstance().getConnectedCar())
				{
					float v = y;

					if (true == AppSettings.getInstance().isInvertThrottle())
					{
						v *= -1.0f;
					}

					int throttleCenter = CarsController.getInstance().getConnectedCar().getCenterThrottle();
					int throttleMin = CarsController.getInstance().getConnectedCar().getMinThrottle();
					int throttleMax = CarsController.getInstance().getConnectedCar().getMaxThrottle();

					int nds = throttleCenter - throttleMin;
					int pds = throttleMax - throttleCenter;

					int throttle = (int) (throttleCenter + ((v < 0) ? (nds * v * 2) : (pds * v * 2)));

					CarsController.getInstance().getConnectedCar().setThrottle(throttle);
				}
			};
		});

		this.thumbStickSteering = (ThumbStickView) findViewById(R.id.thumbStickViewSteering);
		this.thumbStickSteering.setEnabled(false);
		this.thumbStickSteering.setArrowImage((Target.current == AppType.BleZee) ? R.drawable.steering_arrow_b : R.drawable.steering_arrow);
		this.thumbStickSteering.setVertical(true);
		this.thumbStickSteering.setValueChangedHandler(new IValueChanged()
		{
			@Override
			public void onValueChanged(float x, float y)
			{
				if ((null != CarsController.getInstance().getConnectedCar()) && (false == AppSettings.getInstance().isUseAccelerometer()))
				{
					float v = x;

					if (true == AppSettings.getInstance().isInvertSteering())
					{
						v *= -1.0f;
					}

					int steeringCenter = CarsController.getInstance().getConnectedCar().getCenterSteering();
					int steeringMin = CarsController.getInstance().getConnectedCar().getMinSteering();
					int steeringMax = CarsController.getInstance().getConnectedCar().getMaxSteering();

					int nds = steeringCenter - steeringMin;
					int pds = steeringMax - steeringCenter;

					int steering = (int) (steeringCenter + ((v < 0) ? (nds * v * 2) : (pds * v * 2)));

					CarsController.getInstance().getConnectedCar().setSteering(steering);
				}
			};
		});

		this.sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
		this.accelerometer = this.sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		this.sensorManager.registerListener(this, this.accelerometer, SensorManager.SENSOR_DELAY_GAME);
		
		updateControlls(CarsController.getInstance().getConnectedCar());
	};

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		if (R.id.menu_connect == item.getItemId())
		{
			Intent newIntent = new Intent(this, Target.deviceScanClass);
			startActivityForResult(newIntent, MainActivity.REQUEST_SELECT_DEVICE);
			overridePendingTransition(R.anim.activity_in, R.anim.activity_out);
			return true;
		}

		final ICarController car = CarsController.getInstance().getConnectedCar();

		if (null == car)
		{
			return super.onOptionsItemSelected(item);
		}

		switch (item.getItemId())
		{
			case R.id.menu_disconnect:
				this.acticeDeviceAddress = null;
				car.setMainLights(false);
				car.setBackLights(false);
				car.setLeftTurnLights(false);
				car.setRightTurnLights(false);
				car.setReverseLights(false);
				
				new Thread(new Runnable()
				{
					@Override
					public void run()
					{
						try
						{
							// give some time for lights off to process...
							Thread.sleep(1000);
						}
						catch (InterruptedException e)
						{
							e.printStackTrace();
						}
						
						car.disconnect();
						
						updateControlls(null);
						invalidateOptionsMenu();						
					}
				}).start();
				
				updateControlls(null);
				invalidateOptionsMenu();
				return true;
			case R.id.menu_settings:
				startActivityForResult(new Intent(this, Target.settingsClass), MainActivity.REQUEST_CHANGE_SETTINGS);
				overridePendingTransition(R.anim.activity_in, R.anim.activity_out);
				return true;
			case R.id.menu_device_info:
				startActivityForResult(new Intent(this, Target.firmwareClass), MainActivity.REQUEST_UPDATE_FIRMWARE);
				overridePendingTransition(R.anim.activity_in, R.anim.activity_out);
				return true;
			case R.id.menu_battery_100:
				int maxVoltage = car.getMaxBatteryVoltage();
				int currentVoltage = car.getBatteryVoltage();
				int batteryLevel = (maxVoltage > 0) ? ((100 * currentVoltage) / maxVoltage) : (100);

				if (batteryLevel > 0)
				{
					showMessage("Battery info", "Battery level: " + batteryLevel + "%");
				}
				else
				{
					showMessage("Battery info", "No data.");
				}
				return true;
			case R.id.menu_left_turn:
				car.setLeftTurnLights(this.leftTurnLight.swapState());
				return true;
			case R.id.menu_right_turn:
				car.setRightTurnLights(this.rightTurnLight.swapState());
				return true;
			case R.id.menu_lights:
				boolean stateMainLights = this.mainLights.swapState();
				car.setMainLights(stateMainLights);
				car.setBackLights(stateMainLights);
				return true;
			case R.id.menu_emergency:
				boolean stateEmergency = this.emergencyLight.swapState();
				car.setRightTurnLights(stateEmergency);
				car.setLeftTurnLights(stateEmergency);
				return true;
			case R.id.menu_bt_ok:
				showMessage("Connection info", "latency: " + (int) (car.getLatency() / 1000000) + "ms");
				return true;
			default:
				return super.onOptionsItemSelected(item);
		}
	};

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.activity_main, menu);

		boolean hasLights = false;

		if (null != CarsController.getInstance().getConnectedCar())
		{
			menu.findItem(R.id.menu_connect).setVisible(false);
			menu.findItem(R.id.menu_disconnect).setVisible(true);
			menu.findItem(R.id.menu_settings).setVisible(true);
			menu.findItem(R.id.menu_device_info).setVisible(true);

			hasLights = CarsController.getInstance().getConnectedCar().hasLights();
		}
		else
		{
			menu.findItem(R.id.menu_connect).setVisible(true);
			menu.findItem(R.id.menu_disconnect).setVisible(false);
			menu.findItem(R.id.menu_settings).setVisible(false);
			menu.findItem(R.id.menu_device_info).setVisible(false);

			hasLights = false;
		}

		this.leftTurnLight = updateLight(this.leftTurnLight, menu.findItem(R.id.menu_left_turn), hasLights);
		this.rightTurnLight = updateLight(this.rightTurnLight, menu.findItem(R.id.menu_right_turn), hasLights);
		this.emergencyLight = updateLight(this.emergencyLight, menu.findItem(R.id.menu_emergency), hasLights);
		this.mainLights = updateLight(this.mainLights, menu.findItem(R.id.menu_lights), hasLights);

		this.batteryStatus = menu.findItem(R.id.menu_battery_100);
		this.connectionStatus = menu.findItem(R.id.menu_bt_ok);

		this.batteryStatus.setVisible(null != CarsController.getInstance().getConnectedCar());
		this.connectionStatus.setVisible(null != CarsController.getInstance().getConnectedCar());

		return true;
	};
	
	private Light updateLight(Light light, MenuItem item, boolean hasLights)
	{
		if (null == light)
		{
			return new Light(item, hasLights);
		}
		else
		{
			light.update(item, hasLights);
			
			return light;
		}
	};

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (MainActivity.REQUEST_SELECT_DEVICE == requestCode)
		{
			if (resultCode == Activity.RESULT_OK)
			{
				updateControlls(null);
				
				String deviceAddress = data.getExtras().getString(BluetoothDevice.EXTRA_DEVICE);
				String deviceName = data.getExtras().getString(BluetoothDevice.EXTRA_NAME);
				
				connect(deviceAddress, deviceName);
			}
			else
			{
				updateControlls(CarsController.getInstance().getConnectedCar());
			}
		}
		else if (MainActivity.REQUEST_CHANGE_SETTINGS == requestCode)
		{
			this.thumbStickSteering.setEnabled(!AppSettings.getInstance().isUseAccelerometer());
			updateControlls(CarsController.getInstance().getConnectedCar());
		}
		else if (REQUEST_UPDATE_FIRMWARE == requestCode)
		{
			if (resultCode == Activity.RESULT_OK)
			{
				showMessage("Success", "Firmware has been uploaded. Please reconnect to the car.");
			}
		}
	};

	private void connect(final String deviceAddress, final String name)
	{
		updateControlls(null);
		
		this.acticeDeviceAddress = deviceAddress;

		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				progress = ProgressDialog.show(MainActivity.this, "", "Connecting to " + (name != null ? name : deviceAddress), true);
			}
		});

		try
		{
			CarsController.getInstance().connect(deviceAddress, (name != null ? name : deviceAddress), this, new IConnectionHandler()
			{
				@Override
				public void onDisconnected(final Object client, final String message)
				{
					updateControlls(null);

					if (null != message)
					{
						MainActivity.this.acticeDeviceAddress = null;
						showMessage("Connect error: " + client.toString(), message);
					}
					else if (MainActivity.this.acticeDeviceAddress != null)
					{
						// connectLE(acticeDeviceAddress);
					}
				};

				@Override
				public void onConnected(final ICarController car)
				{
					updateControlls(car);
					showToast("Connected to: " + car.getName());

					mainLights.setState(true);
					car.setMainLights(true);
					car.setBackLights(true);
					
					emergencyLight.setState(false);
					car.setLeftTurnLights(false);
					car.setRightTurnLights(false);
					car.setReverseLights(false);
					
					leftTurnLight.setState(false);
					rightTurnLight.setState(false);
				};

				@Override
				public void onStatsChanged(ICarController car)
				{
					updateStats();
				};
			});
		}
		catch (IOException ex)
		{
			showMessage("BT connect error: " + deviceAddress, ex.getMessage());
		}
	};

	private void updateControlls(final ICarController car)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				if (null != car)
				{
					setTitle(car.getName());
					MainActivity.this.thumbStickSteering.setEnabled(!AppSettings.getInstance().isUseAccelerometer());
					MainActivity.this.thumbStickThrottle.setEnabled(true);

					if (null != MainActivity.this.connectionStatus)
					{
						MainActivity.this.connectionStatus.setVisible(true);
					}

					if (null != MainActivity.this.batteryStatus)
					{
						MainActivity.this.batteryStatus.setVisible(true);
					}

					hideSoftareButtons(true);
					invalidateOptionsMenu();

					if (null != MainActivity.this.progress)
					{
						MainActivity.this.progress.dismiss();
						MainActivity.this.progress = null;
					}
				}
				else
				{
					if (Target.current == AppType.SmartRacer)
					{
						setTitle(R.string.smart_racer_name);
					}
					else
					{
						setTitle(R.string.ble_zee_name);
					}

					MainActivity.this.thumbStickSteering.setEnabled(false);
					MainActivity.this.thumbStickThrottle.setEnabled(false);

					if (null != MainActivity.this.connectionStatus)
					{
						MainActivity.this.connectionStatus.setVisible(false);
					}

					if (null != MainActivity.this.batteryStatus)
					{
						MainActivity.this.batteryStatus.setVisible(false);
					}

					if (null != MainActivity.this.progress)
					{
						MainActivity.this.progress.dismiss();
						MainActivity.this.progress = null;
					}

					invalidateOptionsMenu();
				}
			};
		});
	};

	private void updateStats()
	{
		runOnUiThread(new Runnable()
		{
			@Override
			@SuppressWarnings("deprecation")
			public void run()
			{

				if (null != CarsController.getInstance().getConnectedCar())
				{
					int maxVoltage = CarsController.getInstance().getConnectedCar().getMaxBatteryVoltage();
					int currentVoltage = CarsController.getInstance().getConnectedCar().getBatteryVoltage();

					int batteryLevel = (maxVoltage > 0) ? ((100 * currentVoltage) / maxVoltage) : (100);

					if (batteryLevel < 25)
					{
						MainActivity.this.batteryStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_battery0));
					}
					else if (batteryLevel < 50)
					{
						MainActivity.this.batteryStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_battery25));

					}
					else if (batteryLevel < 75)
					{
						MainActivity.this.batteryStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_battery50));
					}
					else if (batteryLevel < 100)
					{
						MainActivity.this.batteryStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_battery75));
					}
					else
					{
						MainActivity.this.batteryStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_battery100));
					}

					int latency = (int) (CarsController.getInstance().getConnectedCar().getLatency() / 1000000);

					if (latency < 0)
					{
						MainActivity.this.connectionStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_bt_off));
					}
					else if (latency < 29)
					{
						MainActivity.this.connectionStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_bt_ok));
					}
					else
					{
						MainActivity.this.connectionStatus.setIcon(getResources().getDrawable(R.drawable.ic_action_bt_slow));
					}
				}
			};
		});
	};

	private void hideSoftareButtons(boolean hide)
	{
		if (true == hide)
		{
			// getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
		}
	};

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{
	};

	@Override
	public void onSensorChanged(SensorEvent event)
	{
		if (false == AppSettings.getInstance().isUseAccelerometer())
		{
			return;
		}

		float y = event.values[1];

		switch (AppSettings.getInstance().getAccelerometerAxis())
		{
			case X:
				y = event.values[0];
				break;
			case Y:
				y = event.values[1];
				break;
			case Z:
				y = event.values[2];
				break;
			default:
				break;
		}

		if (null != CarsController.getInstance().getConnectedCar())
		{
			if (y < -10.f)
			{
				y = -10.0f;
			}

			if (y > 10.0f)
			{
				y = 10.0f;
			}

			y = (0.5f + ((y / 10.0f) * 0.5f));

			float v = y - 0.5f;

			if (v < -0.5f)
			{
				v = -0.5f;
			}

			if (v > 0.5f)
			{
				v = 0.5f;
			}

			if (AppSettings.getInstance().isInvertSteering())
			{
				v *= -1;
			}

			this.thumbStickSteering.setPosition(v, 0.0f);

			int steeringCenter = CarsController.getInstance().getConnectedCar().getCenterSteering();
			int steeringMin = CarsController.getInstance().getConnectedCar().getMinSteering();
			int steeringMax = CarsController.getInstance().getConnectedCar().getMaxSteering();

			int nds = steeringCenter - steeringMin;
			int pds = steeringMax - steeringCenter;

			int steering = (int) (steeringCenter + ((v < 0) ? (nds * v * 2) : (pds * v * 2)));

			CarsController.getInstance().getConnectedCar().setSteering(steering);
		}
	};

	@Override
	protected void onPause()
	{
		unregisterReceiver(this.wifiStateReceiver);		
		
		super.onPause();

		if (null != this.sensorManager)
		{
			this.sensorManager.unregisterListener(this);
		}
	};

	@Override
	protected void onResume()
	{
		super.onResume();

		IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
	  intentFilter.addAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
	  intentFilter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
	  intentFilter.addAction(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION);		
		
		registerReceiver(this.wifiStateReceiver, intentFilter);		
		
		if ((null != this.sensorManager) && (null != this.accelerometer))
		{
			this.sensorManager.registerListener(this, this.accelerometer, SensorManager.SENSOR_DELAY_GAME);
		}
	};

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if (keyCode == KeyEvent.KEYCODE_BACK)
		{
			// return true;
		}

		return super.onKeyDown(keyCode, event);
	};

	private void showMessage(final String title, final String message)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				AlertDialog alertDialog = new AlertDialog.Builder(MainActivity.this).create();
				alertDialog.setTitle(title);
				alertDialog.setMessage(message);

				alertDialog.setButton(DialogInterface.BUTTON_POSITIVE, "Ok", new DialogInterface.OnClickListener()
				{
					@Override
					public void onClick(DialogInterface dialog, int whichButton)
					{
					}
				});

				alertDialog.show();
			};
		});
	};
	
	private void showToast(final String message)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				Toast.makeText(getApplicationContext(), message, Toast.LENGTH_SHORT).show();
			};
		});
	};
	
	private void getOverflowMenu()
	{
		try
		{
			ViewConfiguration config = ViewConfiguration.get(this);
			Field menuKeyField = ViewConfiguration.class.getDeclaredField("sHasPermanentMenuKey");
			if (menuKeyField != null)
			{
				menuKeyField.setAccessible(true);
				menuKeyField.setBoolean(config, false);
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	};

	private String acticeDeviceAddress = null;
	private ThumbStickView thumbStickThrottle = null;
	private ThumbStickView thumbStickSteering = null;
	private ProgressDialog progress = null;
	private SensorManager sensorManager = null;
	private Sensor accelerometer = null;
	private static final int REQUEST_SELECT_DEVICE = 2548;
	private static final int REQUEST_CHANGE_SETTINGS = 3;
	private static final int REQUEST_UPDATE_FIRMWARE = 4;
	private Light mainLights = null;
	private Light leftTurnLight = null;
	private Light rightTurnLight = null;
	private Light emergencyLight = null;
	private MenuItem batteryStatus = null;
	private MenuItem connectionStatus = null;
	private BroadcastReceiver wifiStateReceiver;
};
