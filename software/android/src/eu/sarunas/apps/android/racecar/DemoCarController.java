package eu.sarunas.apps.android.racecar;

import java.util.Random;
import android.app.Activity;
import eu.sarunas.apps.android.racecar.firmware.FirmwareUpdater;
import eu.sarunas.apps.android.racecar.firmware.IUpdateProgressListener;

class DemoCarController extends ICarController
{
	protected DemoCarController(String name)
	{
		super(null, DeviceType.MiniZ);

		this.name = name;

		this.updater = new FirmwareUpdater()
		{
			@Override
			public void startUpdate(byte[] fileBuffer, final IUpdateProgressListener progress)
			{
				this.canceled = false;

				new Thread(new Runnable()
				{
					@Override
					public void run()
					{
						for (int i = 0; (i < 100) && (false == canceled); i++)
						{
							progress.onProgressed(i / 100.0f, (100 - i) * 0.5f);
							sleep(100);
						}

						if (false == canceled)
						{
							progress.onFinished();
						}
					};
				}).start();
			};

			@Override
			public void cancelUpdate()
			{
				this.canceled = true;
			};

			private boolean canceled = false;
		};
	};

	public DemoCarController(String address, String name, Activity activity, final IConnectionHandler connectionHandler)
	{
		this(name);

		this.connectionHandler = connectionHandler;

		new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				sleep(1000);
				connectionHandler.onConnected(DemoCarController.this);
			};
		}).run();
	};

	@Override
	public void disconnect()
	{
		new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				sleep(10);
				DemoCarController.this.connectionHandler.onDisconnected(this, null);
			};
		}).run();
	};

	@Override
	public String getAddress()
	{
		return DemoCarController.emptyAddress;
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
		return 16;
	};

	@Override
	public int getCenterSteering()
	{
		return this.centerSteering;
	};

	@Override
	public int getCenterThrottle()
	{
		return this.centerThrottle;
	};

	@Override
	public int getCurrentUsage()
	{
		return 0;
	};

	@Override
	public Object getHandle()
	{
		return null;
	};

	@Override
	public long getLatency()
	{
		return (long) (5.0 + (this.random.nextGaussian() * 15.0));
	};

	@Override
	public int getMaxBatteryVoltage()
	{
		return 0;
	};

	@Override
	public int getMaxSteering()
	{
		return this.maxSteering;
	};

	@Override
	public int getMaxThrottle()
	{
		return this.maxThrottle;
	};

	@Override
	public int getMinSteering()
	{
		return this.minSteering;
	};

	@Override
	public int getMinThrottle()
	{
		return this.minThrottle;
	};

	@Override
	public String getName()
	{
		return this.name;
	};

	@Override
	public int getPulseWidth()
	{
		return 0;
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
		this.centerSteering = centerSteering;
	};

	@Override
	public void setCenterThrottle(int centerThrottle)
	{
		this.centerThrottle = centerThrottle;
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
		this.maxSteering = maxSteering;
	};

	@Override
	public void setMaxThrottle(int maxThrottle)
	{
		this.maxThrottle = maxThrottle;
	};

	@Override
	public void setMinSteering(int minSteering)
	{
		this.minSteering = minSteering;
	};

	@Override
	public void setMinThrottle(int minThrottle)
	{
		this.minThrottle = minThrottle;
	};

	@Override
	public void setName(String name)
	{
		this.name = name;
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
	};

	@Override
	public void setThrottle(int value)
	{
	};

	private void sleep(long time)
	{
		try
		{
			Thread.sleep(time);
		}
		catch (InterruptedException ignored)
		{
		}
	};

	@Override
	public FirmwareUpdater getUpdater()
	{
		return this.updater;
	};

	public static String emptyAddress = "00:00:00:00:00:00";
	private int centerSteering = 700;
	private int centerThrottle = 350;
	private int maxSteering = 1000;
	private int maxThrottle = 700;
	private int minSteering = 400;
	private int minThrottle = 0;
	private String name;
	private Random random = new Random();
	private IConnectionHandler connectionHandler;
	private FirmwareUpdater updater;
};
