package eu.sarunas.apps.android.racecar.bluetooth;

import java.io.Closeable;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import eu.sarunas.apps.android.racecar.controller.DeviceType;
import eu.sarunas.apps.android.racecar.controller.ICarController;
import eu.sarunas.apps.android.racecar.controller.IConnectionHandler;
import eu.sarunas.apps.android.racecar.firmware.IFirmwareUpdater;
import eu.sarunas.apps.android.racecar.wifi.WiFiFirmwareUpdater;

public class BluetoothController extends ICarController
{
	public BluetoothController(String address, IConnectionHandler connectionHandler) throws IOException
	{
		super(address, DeviceType.Simple);

		connect(address, connectionHandler);
	};

	private void close(Closeable stream)
	{
		if (null != stream)
		{
			try
			{
				stream.close();
			}
			catch (IOException ex)
			{
			}
		}
	};

	private void connect(final String blueToothAddress, final IConnectionHandler connectionHandler) throws IOException
	{
		Thread thread = new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				try
				{
					BluetoothController.this.device = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(blueToothAddress);
					BluetoothController.this.socket = BluetoothController.this.device.createInsecureRfcommSocketToServiceRecord(BluetoothController.serialUUID);

					BluetoothController.this.socket.connect();

					BluetoothController.this.inputStream = BluetoothController.this.socket.getInputStream();
					BluetoothController.this.outputStream = BluetoothController.this.socket.getOutputStream();

					byte[] buffer = new byte[1024];

					connectionHandler.onConnected(BluetoothController.this);

					while (true == BluetoothController.this.active)
					{
						try
						{
							if (inputStream.available() > 0)
							{
								int bytes = BluetoothController.this.inputStream.read(buffer);

								if (bytes > 0)
								{
									onDataReceived(buffer, bytes);
								}
							}
						}
						catch (Throwable ex)
						{
							ex.printStackTrace();

							BluetoothController.this.active = false;
						}

						sendData(new byte[] { BluetoothController.this.state });

						try
						{
							Thread.sleep(50);
						}
						catch (InterruptedException e)
						{
							e.printStackTrace();
						}
					}

					connectionHandler.onDisconnected(BluetoothController.this, null);

					close(BluetoothController.this.socket);
				}
				catch (IOException ex)
				{
					ex.printStackTrace();

					connectionHandler.onDisconnected(BluetoothController.this, ex.getMessage());
				}
			}
		});

		thread.start();
	};

	@Override
	public void disconnect()
	{
		this.active = false;

		if (null != this.connectionHandler)
		{
			this.connectionHandler.onDisconnected(this, null);
			this.connectionHandler = null;
		}

		close(this.socket);
		close(this.inputStream);
		close(this.outputStream);
	};

	@Override
	public int getBatteryCapacity()
	{
		return 0;
	};

	@Override
	public int getBatteryVoltage()
	{
		return 0;
	};

	@Override
	public int getCapabilities()
	{
		return 0;
	};

	@Override
	public int getCenterSteering()
	{
		return 350;
	};

	@Override
	public int getCenterThrottle()
	{
		return 350;
	};

	@Override
	public int getCurrentUsage()
	{
		return 0;
	};

	private String getDeviceName()
	{
		if (null != this.device)
		{
			return this.device.getName() + " (" + this.device.getAddress() + ")";
		}

		return "";
	};

	@Override
	public long getLatency()
	{
		return 0;
	};

	@Override
	public int getMaxBatteryVoltage()
	{
		return 0;
	};

	@Override
	public int getMaxSteering()
	{
		return 700;
	};

	@Override
	public int getMaxThrottle()
	{
		return 700;
	};

	@Override
	public int getMinSteering()
	{
		return 0;
	};

	@Override
	public int getMinThrottle()
	{
		return 0;
	};

	@Override
	public String getName()
	{
		return getDeviceName();
	};

	@Override
	public int getPulseWidth()
	{
		return 0;
	};

	@Override
	public IFirmwareUpdater getUpdater()
	{
		return this.updater;
	};

	private void onDataReceived(byte[] data, int length)
	{
		if (data.length > 0)
		{
			try
			{
				String ss = new String(data, "UTF-8");
				System.out.println("[" + ss + "]");
			}
			catch (Throwable ex)
			{
				ex.printStackTrace();
			}
		}
	};

	private void sendData(byte[] bytes)
	{
		try
		{
			if (null != this.outputStream)
			{
				this.outputStream.write(bytes);
				//System.out.println("Sending " + bytes[0]);
			}
		}
		catch (IOException ex)
		{
			this.active = false;

			ex.printStackTrace();
		}
	};

	public void sendTVoff()
	{
		sendData(new byte[] { (byte) 0xff });
	};

	@Override
	public void setBackLights(boolean on)
	{
	};

	@Override
	public void setBatteryCapacity(int batteryCapacity)
	{
	};

	@Override
	public void setCenterSteering(int centerSteering)
	{
	};

	@Override
	public void setCenterThrottle(int centerThrottle)
	{
	};

	@Override
	public void setLeftTurnLights(boolean on)
	{
	};

	@Override
	public void setMainLights(boolean on)
	{
	};

	@Override
	public void setMaxBatteryVoltage(int maxBatteryVoltage)
	{
	};

	@Override
	public void setMaxSteering(int maxSteering)
	{
	};

	@Override
	public void setMaxThrottle(int maxThrottle)
	{
	};

	@Override
	public void setMinSteering(int minSteering)
	{
	};

	@Override
	public void setMinThrottle(int minThrottle)
	{
	};

	@Override
	public void setName(String name)
	{
	};

	@Override
	public void setPulseWidth(int pulseWidth)
	{
	};

	@Override
	public void setReverseLights(boolean on)
	{
	};

	@Override
	public void setRightTurnLights(boolean on)
	{
	};

	@Override
	public void setSteering(int value)
	{
		this.state &= ~(1);
		this.state &= ~(2);

		if (value > getCenterSteering())
		{
			this.state |= 1;
		}
		else if (value < getCenterSteering())
		{
			this.state |= 2;
		}
	};

	@Override
	public void setThrottle(int value)
	{
		this.state &= ~(4);
		this.state &= ~(8);

		if (value > getCenterThrottle())
		{
			this.state |= 4;
		}
		else if (value < getCenterThrottle())
		{
			this.state |= 8;
		}
	};

	public void soundHorn()
	{
		sendData(new byte[] { 16 });
	};

	public static final String btPrefix = "BT-";
	private static final UUID serialUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");;
	private boolean active = true;;
	private IConnectionHandler connectionHandler = null;;
	private BluetoothDevice device = null;;
	private InputStream inputStream = null;;
	private OutputStream outputStream = null;;
	private BluetoothSocket socket = null;;
	private byte state = 0;;
	private WiFiFirmwareUpdater updater = new WiFiFirmwareUpdater();;
};
