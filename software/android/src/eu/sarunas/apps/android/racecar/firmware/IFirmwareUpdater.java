package eu.sarunas.apps.android.racecar.firmware;

import eu.sarunas.apps.android.racecar.ble.CharacteristicProxy;

public abstract class IFirmwareUpdater
{
	public abstract String getHardwareVersion();
	public abstract String getFirmwareVersion();
	public abstract void cancelUpdate();
	public abstract void startUpdate(byte[] mFileBuffer, IUpdateProgressListener progress);
	public abstract short getCurrentImageVersion();
	public abstract ImageHeader getHeader(byte[] fileBuffer);
	public abstract CharacteristicProxy getImage();

	public UpdateState state = UpdateState.Ready;
};
