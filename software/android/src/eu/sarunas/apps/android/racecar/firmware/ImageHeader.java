package eu.sarunas.apps.android.racecar.firmware;

public class ImageHeader
{
	public short ver;
	public short len;
	public Character imgType;
	public byte[] uid = new byte[4];
};
