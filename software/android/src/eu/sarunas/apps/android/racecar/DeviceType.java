package eu.sarunas.apps.android.racecar;

public enum DeviceType
{
	MiniZ(16, "mini-z-"), RC(32, "rc-"), Simple(64, "rc-simple-"), Unknown(0xff, ""), MiniZBLDC(1 << 10, "mini-z-bldc-"), dNano(1 << 11, "dnano-");

	DeviceType(int id, String firmwarePrefix)
	{
		this.typeId = id;
		this.firmwarePrefix = firmwarePrefix;
	};

	public static DeviceType getType(int id)
	{
		for (DeviceType type : DeviceType.values())
		{
			if ((id & type.typeId) > 0)
			{
				return type;
			}
		}

		return RC;
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
			default:
				return MiniZ;
		}
	};

	public String getFirmwarePrefix()
	{
		return this.firmwarePrefix;
	};

	public int getId()
	{
		return this.typeId;
	};

	private String firmwarePrefix;
	private int typeId;
};
