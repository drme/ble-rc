package eu.sarunas.apps.android.racecar.scanner;

public interface IScanCallback
{
	public void onCarFound(DiscoveredDevice car);
	public void onScanStarted();
	public void onScanFinished();
	public void onError(int messageId);
};
