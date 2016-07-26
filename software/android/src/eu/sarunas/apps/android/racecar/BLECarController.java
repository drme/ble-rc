package eu.sarunas.apps.android.racecar;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.util.Log;
import eu.sarunas.apps.android.racecar.ble.CharacteristicProxy;
import eu.sarunas.apps.android.racecar.firmware.FirmwareUpdater;
import eu.sarunas.apps.android.racecar.firmware.UpdateState;

@SuppressLint("NewApi")
public class BLECarController extends ICarController
{
	public BLECarController(BluetoothDevice device, String name, DeviceType type)
	{
		super(device.getAddress(), type);

		this.device = device;
		this.advertisementName = name;
	}

	public BLECarController(String address, Activity activity, DeviceType type, IConnectionHandler connectionHandler) throws IOException
	{
		super(address, type);

		assert (null != connectionHandler);
		assert (null != activity);
		assert (null != address);

		this.lightsController = new LightsController(this.lights);
		this.connectionHandler = connectionHandler;
		this.bluetoothManager = (BluetoothManager) activity.getSystemService(Context.BLUETOOTH_SERVICE);

		if (null == this.bluetoothManager)
		{
			throw new IOException("Unable to initialize BluetoothManager.");
		}

		this.bluetoothAdapter = this.bluetoothManager.getAdapter();

		if (null == this.bluetoothAdapter)
		{
			throw new IOException("Unable to obtain a BluetoothAdapter.");
		}

		BluetoothDevice device = this.bluetoothAdapter.getRemoteDevice(address);

		if (null == device)
		{
			throw new IOException("Device not found. Unable to connect.");
		}

		this.bluetoothGatt = device.connectGatt(activity, false, this.gattCallback);

		if (null == this.bluetoothGatt)
		{
			throw new IOException("Unable to connect.");
		}
	};

	private final BluetoothGattCallback gattCallback = new BluetoothGattCallback()
	{
		@Override
		public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState)
		{
			if (newState == BluetoothProfile.STATE_CONNECTED)
			{
				if (bluetoothGatt != null)
				{
				
				boolean started = bluetoothGatt.discoverServices();

				Log.i(TAG, "Connected to GATT server.");
				Log.i(TAG, "Attempting to start service discovery: " + started);
				}
				else
				{
					connectionHandler.onDisconnected(null, null);
					disconnectCharacteristics();
					disconnect();
				}
			}
			else if (newState == BluetoothProfile.STATE_DISCONNECTED)
			{
				Log.i(TAG, "Disconnected from GATT server.");

				// .. if (bluetoothGatt != null)
				// .. {
				// . bluetoothGatt = device.connectGatt(activity, false, this.gattCallback);
				// }

				connectionHandler.onDisconnected(null, null);

				disconnectCharacteristics();

				disconnect();
			}
		};

		@Override
		public void onServicesDiscovered(BluetoothGatt gatt, int status)
		{
			if (status == BluetoothGatt.GATT_SUCCESS)
			{
				connectCharacteristics(gatt);

				new Thread(new Runnable()
				{
					@Override
					public void run()
					{
						sleep(100);
						CharacteristicProxy.commandsQueue.setBlockCommands(false);
						sleep(2000);
						startControllThreads();
					};
				}).start();

			}
			else
			{
				Log.w(TAG, "onServicesDiscovered received: " + status);
			}
		};

		@Override
		public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status)
		{
			updateCharacteristic(characteristic);
		};

		@Override
		public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic)
		{
			updateCharacteristic(characteristic);
		};

		@Override
		public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status)
		{
		};

		@Override
		public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status)
		{
		};

		@Override
		public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status)
		{
			for (CharacteristicProxy proxy : characteristics)
			{
				proxy.checkDescriptor(descriptor, status);
			}
		};
	};

	private void registerCharacteristic(BluetoothGatt gatt, CharacteristicProxy proxy)
	{
		proxy.connect(gatt);

		this.characteristics.add(proxy);
	};

	private void connectCharacteristics(BluetoothGatt gatt)
	{
		registerCharacteristic(gatt, this.deviceName);

		registerCharacteristic(gatt, this.allDrive);
		registerCharacteristic(gatt, this.steering);
		registerCharacteristic(gatt, this.throttle);

		registerCharacteristic(gatt, this.centerThrottle);
		registerCharacteristic(gatt, this.maxThrottle);
		registerCharacteristic(gatt, this.minThrottle);

		registerCharacteristic(gatt, this.centerSteering);
		registerCharacteristic(gatt, this.maxSteering);
		registerCharacteristic(gatt, this.minSteering);

		registerCharacteristic(gatt, this.batteryMaxVoltage);
		registerCharacteristic(gatt, this.batteryCapacity);
		registerCharacteristic(gatt, this.batteryVoltage);
		registerCharacteristic(gatt, this.currentUsage);

		registerCharacteristic(gatt, this.pulseWidth);

		registerCharacteristic(gatt, this.lights);

		registerCharacteristic(gatt, this.capabilities);

		registerCharacteristic(gatt, this.updater.getBlock());
		registerCharacteristic(gatt, this.updater.getImage());

		registerCharacteristic(gatt, this.updater.getFirmware());
		registerCharacteristic(gatt, this.updater.getHardware());
	};

	private void disconnectCharacteristics()
	{
		for (CharacteristicProxy proxy : this.characteristics)
		{
			proxy.disconnect();
		}
	};

	private void updateCharacteristic(BluetoothGattCharacteristic characteristic)
	{
		for (CharacteristicProxy proxy : this.characteristics)
		{
			proxy.updateValue(characteristic);
		}
	};

	private void startControllThreads()
	{
		Thread statsThread = new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				while (null != bluetoothGatt)
				{
					refreshStats();
					sleep(10000);
				}
			};
		});

		statsThread.start();

		this.oldSteering = -1;// this.centerSteering.getIntValue();
		this.oldThrottle = -1;// this.centerThrottle.getIntValue();

		Thread controllThread = new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				while (null != bluetoothGatt)
				{
					updateControll();
				}
			};
		});

		controllThread.start();

		this.connectionHandler.onConnected(this);
	};

	@Override
	public synchronized void disconnect()
	{
		CharacteristicProxy.commandsQueue.clear();
		this.characteristics.clear();
		
		if (null != this.bluetoothGatt)
		{
			this.bluetoothGatt.disconnect();
			this.bluetoothGatt.close();
		}

		this.bluetoothGatt = null;
		
		connectionHandler.onDisconnected(null, null);		
	};

	@Override
	public void setSteering(int value)
	{
		// if (value != this.oldSteering)
		// {
		// if (Math.abs(value - this.oldSteering) > 30)
		// {
		this.oldSteering = value;

		// writeAttribute(this.steeringId, this.oldSteering);
		// }
		// }
	};

	@Override
	public void setThrottle(int value)
	{
		// if (value != this.oldThrottle)
		// {
		// if (Math.abs(value - this.oldThrottle) > 10)
		// {
		this.oldThrottle = value;

		// writeAttribute(this.throttleId, this.oldThrottle);

		// setReverseLights(value < getCenterThrottle() - 10);
		// }
		// } */
	};

	@Override
	public void setPulseWidth(int pulseWidth)
	{
		this.pulseWidth.writeValue(pulseWidth);
	};

	@Override
	public int getPulseWidth()
	{
		return this.pulseWidth.getIntValue();
	};

	@Override
	public void setMinThrottle(int minThrottle)
	{
		this.minThrottle.writeValue(minThrottle);
	};

	@Override
	public int getMinThrottle()
	{
		return this.minThrottle.getIntValue();
	};

	@Override
	public void setMaxThrottle(int maxThrottle)
	{
		this.maxThrottle.writeValue(maxThrottle);
	};

	@Override
	public int getMaxThrottle()
	{
		return this.maxThrottle.getIntValue();
	};

	@Override
	public void setCenterThrottle(int centerThrottle)
	{
		this.centerThrottle.writeValue(centerThrottle);
	};

	@Override
	public int getCenterThrottle()
	{
		return this.centerThrottle.getIntValue();
	};

	@Override
	public void setCenterSteering(int centerSteering)
	{
		this.centerSteering.writeValue(centerSteering);
	};

	@Override
	public int getCenterSteering()
	{
		return this.centerSteering.getIntValue();
	};

	@Override
	public void setMinSteering(int minSteering)
	{
		this.minSteering.writeValue(minSteering);
	};

	@Override
	public int getMinSteering()
	{
		return this.minSteering.getIntValue();
	};

	@Override
	public void setMaxSteering(int maxSteering)
	{
		this.maxSteering.writeValue(maxSteering);
	};

	@Override
	public int getMaxSteering()
	{
		return this.maxSteering.getIntValue();
	};

	@Override
	public void setMaxBatteryVoltage(int maxBatteryVoltage)
	{
		this.batteryMaxVoltage.writeValue(maxBatteryVoltage);
	};

	@Override
	public int getMaxBatteryVoltage()
	{
		return this.batteryMaxVoltage.getIntValue();
	};

	@Override
	public void setBatteryCapacity(int batteryCapacity)
	{
		this.batteryCapacity.writeValue(batteryCapacity);
	};

	@Override
	public int getBatteryCapacity()
	{
		return this.batteryCapacity.getIntValue();
	};

	@Override
	public int getCurrentUsage()
	{
		return this.currentUsage.getIntValue();
	};

	@Override
	public int getBatteryVoltage()
	{
		return this.batteryVoltage.getIntValue();
	};

	@Override
	public int getCapabilities()
	{
		return this.capabilities.getIntValue();
	};

	@Override
	public String getName()
	{
		if (false == this.deviceName.isAvailable())
		{
			return this.advertisementName;
		}

		return this.deviceName.getStringValue();
	};

	@Override
	public void setName(String name)
	{
		Log.d(TAG, "New name: " + name);

		this.deviceName.writeValue(name);
	};

	@Override
	public long getLatency()
	{
		return this.batteryVoltage.getLatency() / 2;
	};

	private void refreshStats()
	{
		if (this.updater.state != UpdateState.Running)
		{
			this.batteryVoltage.startRead();
			this.connectionHandler.onStatsChanged(this);
		}
	};

	private void updateControll()
	{
		if (this.updater.state != UpdateState.Running)
		{
			if ((-1 == this.oldSteering) && (-1 == this.oldThrottle))
			{
				return;
			}
			if ((true == this.allDrive.isAvailable()) && (this.oldSteering >= 0) && (this.oldThrottle >= 0))
			{
				this.allDrive.writeValue(this.oldThrottle, this.oldSteering);
				sleepByLatency(false);
			}
			else
			{
				if (this.oldSteering >= 0)
				{
					this.steering.writeValue(this.oldSteering);
				}
				if (this.oldThrottle >= 0)
				{
					this.throttle.writeValue(this.oldThrottle);
				}
				sleepByLatency(true);
			}
		}
	};

	private void sleepByLatency(boolean doubleIt)
	{
		int sleepTime = 50; 
				/*(int) ((float) getLatency() * 1.1f / 1000000);

		if (sleepTime > 1000)
		{
			sleepTime = 1000;
		}

		if (sleepTime <= 10)
		{
			sleepTime = 10;
		}

		if (true == doubleIt)
		{
			sleepTime *= 2;
		}*/

		//Log.d(TAG, "Sleeping for: " + sleepTime + "ms");

		sleep(sleepTime);
	};

	private void sleep(int ms)
	{
		try
		{
			Thread.sleep(ms);
		}
		catch (InterruptedException ex)
		{
			ex.printStackTrace();
		}
	};

	@Override
	public void setMainLights(boolean on)
	{
		this.lightsController.setMainLights(on);
	};

	@Override
	public void setLeftTurnLights(boolean on)
	{
		this.lightsController.setLeftTurnLights(on);
	};

	@Override
	public void setRightTurnLights(boolean on)
	{
		this.lightsController.setRightTurnLights(on);
	};

	@Override
	public void setBackLights(boolean on)
	{
		this.lightsController.setBackLights(on);
	};

	@Override
	public void setReverseLights(boolean on)
	{
		this.lightsController.setReverseLights(on);
	};

	@Override
	public Object getHandle()
	{
		return this.device;
	};

	public FirmwareUpdater getUpdater()
	{
		return this.updater;
	};

	private BluetoothDevice device;
	private int oldThrottle = 0;
	private int oldSteering = 0;
	private IConnectionHandler connectionHandler = null;
	private BluetoothManager bluetoothManager = null;
	private BluetoothAdapter bluetoothAdapter = null;
	private BluetoothGatt bluetoothGatt = null;
	private final static String TAG = "RC-BLE";
	private CharacteristicProxy steering = new CharacteristicProxy("acc2", false, 2, false, false);
	private CharacteristicProxy throttle = new CharacteristicProxy("acc1", false, 2, false, false);
	private CharacteristicProxy allDrive = new CharacteristicProxy("acc3", false, 4, false, false);
	private CharacteristicProxy minThrottle = new CharacteristicProxy("b101", true);
	private CharacteristicProxy maxThrottle = new CharacteristicProxy("b102", true);
	private CharacteristicProxy centerThrottle = new CharacteristicProxy("b103", true);
	private CharacteristicProxy centerSteering = new CharacteristicProxy("c103", true);
	private CharacteristicProxy minSteering = new CharacteristicProxy("c101", true);
	private CharacteristicProxy maxSteering = new CharacteristicProxy("c102", true);
	private CharacteristicProxy deviceName = new CharacteristicProxy("f101", true, 20, true, true);
	private CharacteristicProxy batteryMaxVoltage = new CharacteristicProxy("e103", false);
	private CharacteristicProxy batteryCapacity = new CharacteristicProxy("e104", false);
	private CharacteristicProxy batteryVoltage = new CharacteristicProxy("e101", false);
	private CharacteristicProxy currentUsage = new CharacteristicProxy("e102", false);
	private CharacteristicProxy pulseWidth = new CharacteristicProxy("d101", false);
	private CharacteristicProxy lights = new CharacteristicProxy("a101", false);
	private CharacteristicProxy capabilities = new CharacteristicProxy("5001", true);
	private List<CharacteristicProxy> characteristics = new ArrayList<CharacteristicProxy>();
	private LightsController lightsController = null;
	private String advertisementName;
	private FirmwareUpdater updater = new FirmwareUpdater();
};
