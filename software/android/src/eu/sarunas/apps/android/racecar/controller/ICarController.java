package eu.sarunas.apps.android.racecar.controller;

import eu.sarunas.apps.android.racecar.firmware.IFirmwareUpdater;

public abstract class ICarController
{
	protected ICarController(String address, DeviceType type)
	{
		this.address = address;
		this.type = type;
	};

	public abstract void disconnect();

	public abstract void setMainLights(boolean on);

	public abstract void setLeftTurnLights(boolean on);

	public abstract void setRightTurnLights(boolean on);

	public abstract void setBackLights(boolean on);

	public abstract void setReverseLights(boolean on);

	public abstract void setSteering(int value);

	public abstract void setThrottle(int value);

	public abstract String getName();

	public abstract void setName(String name);

	public abstract int getMinThrottle();

	public abstract void setMinThrottle(int minThrottle);

	public abstract int getMaxThrottle();

	public abstract void setMaxThrottle(int maxThrottle);

	public abstract int getCenterThrottle();

	public abstract void setCenterThrottle(int centerThrottle);

	public abstract int getMinSteering();

	public abstract void setMinSteering(int minSteering);

	public abstract int getMaxSteering();

	public abstract void setMaxSteering(int maxSteering);

	public abstract int getCenterSteering();

	public abstract void setCenterSteering(int centerSteering);

	public abstract int getPulseWidth();

	public abstract void setPulseWidth(int pulseWidth);

	public abstract int getMaxBatteryVoltage();

	public abstract void setMaxBatteryVoltage(int maxBatteryVoltage);

	public abstract int getBatteryCapacity();

	public abstract void setBatteryCapacity(int batteryCapacity);

	public abstract int getCurrentUsage();

	public abstract int getBatteryVoltage();

	public abstract int getCapabilities();

	public abstract long getLatency();

	public abstract IFirmwareUpdater getUpdater();

	public void centerSteering()
	{
		setSteering(getCenterSteering());
	};

	public void centerThrottle()
	{
		setThrottle(getCenterThrottle());
	};

	public String getAddress()
	{
		return this.address;
	};

	public DeviceType getType()
	{
		if (this.type == DeviceType.Unknown)
		{
			return DeviceType.Simple;
		}
		else
		{
			return this.type;
		}
	};

	public boolean hasLights()
	{
		int capabilities = getCapabilities();

		return ((capabilities & 8) > 0);
	};

	public int getMaxPosibleSteering()
	{
		if ((DeviceType.MiniZ == getType()) || (DeviceType.MiniZBLDC == getType()) || (DeviceType.dNano == getType()))
		{
			return 2000;
		}
		else
		{
			return 700;
		}
	};

	public int getMaxPosibleThrottle()
	{
		return 700;
	};
	
	private String address = null;
	private DeviceType type = DeviceType.Unknown;
};
