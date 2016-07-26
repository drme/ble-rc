package eu.sarunas.apps.android.racecar.ble;

public interface ICharacteristicDataHandler
{
	public void onDataAvailable(CharacteristicProxy proxy, byte[] data);
	public void onConnected(CharacteristicProxy proxy);
};
