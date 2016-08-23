package eu.sarunas.apps.android.racecar.scanner;

import eu.sarunas.apps.android.racecar.controller.DeviceType;

/**
 * Represents a discovered device during a devices scan.
 */
public class DiscoveredDevice
{
	public DiscoveredDevice(String address, String name, DeviceType type)
	{
		this.address = address;
		this.name = name;
		this.type = type;
	}

	public String getAddress()
	{
		return this.address;
	}

	public String getName()
	{
		return this.name;
	}

	public DeviceType getType()
	{
		return this.type;
	};

	private String address;
	private String name;
	private DeviceType type;
};
