package eu.sarunas.apps.android.racecar.controller;

public interface IConnectionHandler
{
	public void onConnected(ICarController car);
	public void onDisconnected(Object client, String message);
	public void onStatsChanged(ICarController car);
};
