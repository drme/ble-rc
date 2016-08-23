package eu.sarunas.apps.android.racecar.scanner;

import java.util.Arrays;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import eu.sarunas.apps.android.racecar.controller.DeviceType;

public class Scanner
{
	public void start(IScanCallback callBack, final long timeOut, Context context)
	{
		this.callBack = callBack;

		this.callBack.onScanStarted();

		if (timeOut > 0)
		{
			Thread scanTimoutThread = new Thread(new Runnable()
			{
				@Override
				public void run()
				{
					try
					{
						Thread.sleep(timeOut);
					}
					catch (InterruptedException e)
					{
						e.printStackTrace();
					}

					stop();
				};
			});

			scanTimoutThread.setName("Scan time-out thread");
			scanTimoutThread.start();
		}
	};

	public void stop()
	{
		if (this.stopped == false)
		{
			this.stopped = true;

			if (null != this.callBack)
			{
				this.callBack.onScanFinished();
			}
		}
	};

	protected void parseRecord(final BluetoothDevice device, int rssi, byte[] scanRecord)
	{
		boolean valid = false;
		final StringBuilder name = new StringBuilder();
		int index = 0;
		boolean hasFullName = false;
		DeviceType deviceType = DeviceType.Unknown;

		while (index < scanRecord.length)
		{
			int length = scanRecord[index++];

			if (length == 0)
			{
				break;
			}

			int type = scanRecord[index];

			if (type == 0)
			{
				break;
			}

			byte[] data = Arrays.copyOfRange(scanRecord, index + 1, index + length);

			if (type == Scanner.GAP_ADTYPE_16BIT_MORE)
			{
				if ((data.length == 2) && (data[0] == Scanner.serviceId[0]) && (data[1] == Scanner.serviceId[1]))
				{
					valid = true;
				}
			}
			else if (type == Scanner.GAP_ADTYPE_128BIT_MORE)
			{
				if (data.length == Scanner.service128Id.length)
				{
					int matched = 0;

					for (int i = 0; i < data.length; i++)
					{
						if (data[i] == Scanner.service128Id[i])
						{
							matched++;
						}
					}

					valid = matched == data.length;
				}
			}
			else if (type == Scanner.GAP_ADTYPE_LOCAL_NAME_SHORT)
			{
				if (false == hasFullName)
				{
					for (int i = 0; (i < data.length) && (data[i] != 0); i++)
					{
						name.append((char) data[i]);
					}
				}
			}
			else if (type == Scanner.GAP_ADTYPE_LOCAL_NAME_COMPLEATE)
			{
				for (int i = 0; (i < data.length) && (data[i] != 0); i++)
				{
					name.append((char) data[i]);
				}

				hasFullName = true;
			}
			else if (type == Scanner.GAP_ADTYPE_CUSTOM)
			{
				if (data.length == 3)
				{
					deviceType = DeviceType.getTypeFromAdvertisement(data[2]);
				}
			}

			index += length;
		}

		if (true == valid)
		{
			this.callBack.onCarFound(new DiscoveredDevice(device.getAddress(), name.toString(), deviceType));
		}
	};

	private boolean stopped = false;
	protected IScanCallback callBack = null;
	private static int GAP_ADTYPE_16BIT_MORE = 0x02;
	private static int GAP_ADTYPE_LOCAL_NAME_SHORT = 0x08;
	private static int GAP_ADTYPE_LOCAL_NAME_COMPLEATE = 0x09;
	private static int GAP_ADTYPE_CUSTOM = -1;
	private static int GAP_ADTYPE_128BIT_MORE = 0x06;
	private static byte[] serviceId = new byte[] { (byte) 0xc0, (byte) 0xac };
	private static byte[] service128Id = new byte[] { (byte) 0x48, (byte) 0xe0, (byte) 0x0c, (byte) 0x9e, (byte) 0x6d, (byte) 0xd6, (byte) 0x4a, (byte) 0x99, (byte) 0x92, (byte) 0x16, (byte) 0xcd, (byte) 0xf6, (byte) 0xc0, (byte) 0xac, (byte) 0x39, (byte) 0xa7 };
};
