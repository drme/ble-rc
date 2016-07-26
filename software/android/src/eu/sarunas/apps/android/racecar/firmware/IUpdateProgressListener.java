package eu.sarunas.apps.android.racecar.firmware;

public interface IUpdateProgressListener
{
	public void onFailed();
	public void onFinished();
	public void onBadImage();
	public void onProgressed(float percent, float timeLeft);
};
