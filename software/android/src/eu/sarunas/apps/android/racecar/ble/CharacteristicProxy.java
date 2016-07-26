package eu.sarunas.apps.android.racecar.ble;

import java.util.UUID;
import android.annotation.TargetApi;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.util.Log;

@TargetApi(18)
public class CharacteristicProxy
{
	public CharacteristicProxy(String longId, String serviceId, ICharacteristicDataHandler dataHandler)
	{
		this.longId = UUID.fromString(longId);
		this.serviceId = UUID.fromString(serviceId);
		this.dataHandler = dataHandler;
		this.readable = true;
	};

	public CharacteristicProxy(String shortId, boolean required, int bufferSize, boolean readable, boolean readOnLoad)
	{
		this.shortId = shortId;
		this.writeBuffer = new byte[bufferSize];
		this.readable = readable;
		this.readOnLoad = readOnLoad;
	};

	public CharacteristicProxy(String shortId, boolean readOnLoad)
	{
		this(shortId, true, 2, true, readOnLoad);
	};

	public void connect(BluetoothGatt gatt)
	{
		assert (null != gatt);

		this.gatt = gatt;

		BluetoothGattService service = null;

		if (null != this.serviceId)
		{
			service = gatt.getService(this.serviceId);
		}
		else
		{
			service = this.gatt.getService(CharacteristicProxy.serviceLongId);

			if (null == service)
			{
				service = gatt.getService(CharacteristicProxy.serviceShortId);
			}
		}

		if (null == service)
		{
			return;
		}

		if (null != this.longId)
		{
			this.characteristic = service.getCharacteristic(this.longId);
		}
		else
		{
			this.characteristic = service.getCharacteristic(UUID.fromString("a739" + this.shortId + "-f6cd-1692-994a-d66d9e0ce048"));

			if (null == this.characteristic)
			{
				this.characteristic = service.getCharacteristic(UUID.fromString("0000" + this.shortId + "-0000-1000-8000-00805f9b34fb"));
			}
		}

		if (null == this.characteristic)
		{
			Log.e(CharacteristicProxy.logTag, "No characteristic found: " + this.shortId);
		}
		else
		{
			this.characteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE);

			if (null != this.dataHandler)
			{
				this.dataHandler.onConnected(this);
			}

			if (true == this.readOnLoad)
			{
				startRead();
			}
		}
	};

	public void disconnect()
	{
		this.characteristic = null;
		this.gatt = null;
	};

	public boolean isAvailable()
	{
		return (this.characteristic != null);
	};

	public boolean isReadable()
	{
		return this.readable;
	};

	public long getLatency()
	{
		return this.latency;
	};

	public void updateValue(BluetoothGattCharacteristic characteristic)
	{
		if (characteristic == this.characteristic)
		{
			this.latency = System.nanoTime() - this.startTime;

			byte[] data = characteristic.getValue();

			if (null == data)
			{
			}
			else if (data.length == 2)
			{
				this.intValue = ((data[1] << 8) & 0x0000ff00) | (data[0] & 0x000000ff);

				Log.d(CharacteristicProxy.logTag, "Reading: " + this.characteristic.getUuid().toString() + " -> " + this.intValue);
			}
			else if (data.length == 21)
			{
				this.stringValue = "";

				for (byte b : data)
				{
					if (b != 0)
					{
						this.stringValue += (char) b;
					}
				}

				Log.d(CharacteristicProxy.logTag, "Reading: " + this.characteristic.getUuid().toString() + " -> " + this.stringValue);
			}
			else
			{
				Log.d(CharacteristicProxy.logTag, "Reading: " + this.characteristic.getUuid().toString() + " -> " + data.toString());
			}

			if (null != this.dataHandler)
			{
				this.dataHandler.onDataAvailable(this, data);
			}

			this.waitForValue = false;
		}
	};

	public void startRead()
	{
		if (false == this.readable)
		{
			Log.d(CharacteristicProxy.logTag, "read on unreadable: " + this.shortId);
			return;
		}

		if ((null == this.characteristic) || (null == this.gatt))
		{
			Log.d(CharacteristicProxy.logTag, "Fail to read - no char..: " + this.shortId);
			return;
		}

		Runnable request = new Runnable()
		{
			@Override
			public void run()
			{
				System.out.println("Start read for " + "longId.toString()");

				CommandsQueue.sleep(10);
				
				CharacteristicProxy.this.startTime = System.nanoTime();

				CharacteristicProxy.this.waitForValue = true;

				if (false == CharacteristicProxy.this.gatt.readCharacteristic(CharacteristicProxy.this.characteristic))
				{
					Log.d(CharacteristicProxy.logTag, "Failed to read: " + CharacteristicProxy.this.characteristic.getUuid().toString());
					CharacteristicProxy.this.waitForValue = false;
				}
			}
		};

		Runnable response = new Runnable()
		{
			@Override
			public void run()
			{
				while (CharacteristicProxy.this.waitForValue == true)
				{
					CommandsQueue.sleep(100);
				}

				System.out.println("Done read for " + "longId.toString()");
			};
		};

		CharacteristicProxy.commandsQueue.add(request, response);
	};

	public int getIntValue()
	{
		return this.intValue;
	};

	public String getStringValue()
	{
		return this.stringValue;
	};

	public void writeValue(byte[] data)
	{
		if (null != this.characteristic)
		{
			this.characteristic.setValue(data);

			this.gatt.writeCharacteristic(this.characteristic);
		}
		else if (true == this.required)
		{
			Log.e(CharacteristicProxy.logTag, "Write to missing characteristic: " + this.shortId);
		}
	};

	public void writeValue(int v1, int v2)
	{
		//Log.d(CharacteristicProxy.logTag, "Writing: " + this.shortId + " -> " + v1 + "-" + v2);

		this.writeBuffer[0] = (byte) v1;
		this.writeBuffer[1] = (byte) (v1 >> 8);
		this.writeBuffer[2] = (byte) v2;
		this.writeBuffer[3] = (byte) (v2 >> 8);

		writeValue(this.writeBuffer);
	};

	public void writeValue(int value)
	{
		this.intValue = value;

		//Log.d(CharacteristicProxy.logTag, "Writing: " + this.shortId + " -> " + value);

		this.writeBuffer[0] = (byte) value;
		this.writeBuffer[1] = (byte) (value >> 8);

		writeValue(this.writeBuffer);
	};

	public void writeValue(String value)
	{
		this.stringValue = value;

		Log.d(CharacteristicProxy.logTag, "Writing: " + this.shortId + " -> " + value);

		for (int i = 0; (i < (value.length())) && (i < 20); i++)
		{
			this.writeBuffer[i] = (byte) value.charAt(i);
		}

		writeValue(this.writeBuffer);
	};

	public void setNotify(final boolean enable)
	{
		Runnable request = new Runnable()
		{
			@Override
			public void run()
			{
				System.out.println("Start notift on for " + CharacteristicProxy.this.longId.toString());

				CharacteristicProxy.this.waitForDescriptor = true;

				if (true == CharacteristicProxy.this.gatt.setCharacteristicNotification(CharacteristicProxy.this.characteristic, enable))
				{
					BluetoothGattDescriptor clientConfig = CharacteristicProxy.this.characteristic.getDescriptor(CharacteristicProxy.CLIENT_CHARACTERISTIC_CONFIG);

					if (clientConfig == null)
					{
						Log.w(CharacteristicProxy.logTag, "setCharacteristicNotification failed");
						CharacteristicProxy.this.waitForDescriptor = false;
					}

					if (enable)
					{
						clientConfig.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
					}
					else
					{
						clientConfig.setValue(BluetoothGattDescriptor.DISABLE_NOTIFICATION_VALUE);
					}

					if (false == CharacteristicProxy.this.gatt.writeDescriptor(clientConfig))
					{
						Log.w(CharacteristicProxy.logTag, "setCharacteristicNotification failed");
						CharacteristicProxy.this.waitForDescriptor = false;
					}
				}
				else
				{
					Log.w(CharacteristicProxy.logTag, "setCharacteristicNotification failed");
					CharacteristicProxy.this.waitForDescriptor = false;
				}
			}
		};

		Runnable response = new Runnable()
		{
			@Override
			public void run()
			{
				while (CharacteristicProxy.this.waitForDescriptor == true)
				{
					CommandsQueue.sleep(10);
				}

				System.out.println("Got notift on for " + CharacteristicProxy.this.longId.toString());
			}
		};

		CharacteristicProxy.commandsQueue.add(request, response);
	};

	public void checkDescriptor(BluetoothGattDescriptor descriptor, int status)
	{
		if (descriptor.getCharacteristic() == this.characteristic)
		{
			this.waitForDescriptor = false;
		}
	};

	private boolean waitForValue = false;
	private boolean waitForDescriptor = false;
	private long latency = 0;
	private long startTime = 0;
	private String stringValue;
	private int intValue = 0;
	private String shortId = null;
	private UUID longId = null;
	private boolean required = true;
	private boolean readable = false;
	private boolean readOnLoad = false;
	private BluetoothGatt gatt = null;
	private byte[] writeBuffer = null;
	private BluetoothGattCharacteristic characteristic = null;
	private static UUID serviceShortId = UUID.fromString("0000acc0-0000-1000-8000-00805f9b34fb");
	private static UUID serviceLongId = UUID.fromString("a739acc0-f6cd-1692-994a-d66d9e0ce048");
	private static String logTag = "BLE-RC";
	private UUID serviceId = null;
	private ICharacteristicDataHandler dataHandler = null;
	private static final UUID CLIENT_CHARACTERISTIC_CONFIG = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
	public static final CommandsQueue commandsQueue = new CommandsQueue("BLE commands");
};
