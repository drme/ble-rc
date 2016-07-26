package eu.sarunas.apps.android.racecar.ble;

import eu.sarunas.apps.android.racecar.ICarController;

public interface IScanCallback
{
	public void onCarFound(ICarController car);
	public void onScanStarted();
	public void onScanFinished();
	public void onError(int messageId);
};
