package eu.sarunas.apps.android.racecar.controller;

public enum DeviceType
{
	MiniZ("mini-z-"), RC("rc-"), Simple("rc-simple-"), Unknown(""), MiniZBLDC("mini-z-bldc-"), dNano("dnano-");

	DeviceType(String firmwarePrefix)
	{
		this.firmwarePrefix = firmwarePrefix;
	};

	public static DeviceType getTypeFromAdvertisement(int id)
	{
		switch (id)
		{
			case 0:
				return MiniZ;
			case 1:
				return MiniZBLDC;
			case 2:
				return dNano;
			case 3:
				return Simple;
			case 4:
				return RC;
			default:
				return MiniZ;
		}
	};

	public String getFirmwarePrefix()
	{
		return this.firmwarePrefix;
	};

	private String firmwarePrefix;
};
