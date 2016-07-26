package eu.sarunas.apps.android.racecar.wifi;

import eu.sarunas.apps.android.racecar.ble.CharacteristicProxy;
import eu.sarunas.apps.android.racecar.firmware.IFirmwareUpdater;
import eu.sarunas.apps.android.racecar.firmware.IUpdateProgressListener;
import eu.sarunas.apps.android.racecar.firmware.ImageHeader;

public class WiFiFirmwareUpdater extends IFirmwareUpdater
{
	@Override
	public String getHardwareVersion()
	{
		return "N/A";
	};

	@Override
	public String getFirmwareVersion()
	{
		return "N/A";
	};

	@Override
	public void cancelUpdate()
	{
	};

	@Override
	public void startUpdate(byte[] mFileBuffer, IUpdateProgressListener progress)
	{
	};

	@Override
	public short getCurrentImageVersion()
	{
		return 0;
	};

	@Override
	public ImageHeader getHeader(byte[] fileBuffer)
	{
		return null;
	};

	@Override
	public CharacteristicProxy getImage()
	{
		return this.fake;
	};
	
	private CharacteristicProxy fake = new CharacteristicProxy(null, false, 1, false, false);
};
