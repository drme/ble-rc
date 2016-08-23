package eu.sarunas.apps.android.racecar.wifi;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.URL;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import android.app.Activity;
import android.content.Context;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import eu.sarunas.apps.android.racecar.controller.DeviceType;
import eu.sarunas.apps.android.racecar.controller.ICarController;
import eu.sarunas.apps.android.racecar.controller.IConnectionHandler;
import eu.sarunas.apps.android.racecar.firmware.IFirmwareUpdater;

public class WifiCarController extends ICarController
{
	public WifiCarController(String address, String ssid, Activity activity, IConnectionHandler connectionHandler)
	{
		super(address, DeviceType.RC);

		this.activity = activity;
		this.connectionHandler = connectionHandler;

		connectToWiFi(address, ssid, activity);
	};

	public synchronized void connectToServer()
	{
		try
		{
			if (null != this.socket)
			{
				return;
			}
			
			this.socket = new DatagramSocket();
			this.inetAddress = InetAddress.getByName(WifiCarController.defaultIp);

			new Thread(new Runnable()
			{
				@Override
				public void run()
				{
					sleep(1000);
					
					WifiCarController.this.connected = true;

					WifiCarController.this.name.refreshValueString();
					WifiCarController.this.minSteering.refreshValueInt();
					WifiCarController.this.maxSteering.refreshValueInt();
					WifiCarController.this.centerSteering.refreshValueInt();
					WifiCarController.this.minThrottle.refreshValueInt();
					WifiCarController.this.maxThrottle.refreshValueInt();
					WifiCarController.this.centerThrottle.refreshValueInt();
					//WifiCarController.this.batteryStatus.refreshValueInt();

					WifiCarController.this.connectionHandler.onConnected(WifiCarController.this);

					while (true == WifiCarController.this.connected)
					{
						sendValues();

						executeCommands();
						
						sleep(10);
						
						handleLights();
					}
				}
			}).start();
		}
		catch (Throwable ex)
		{
			ex.printStackTrace();

			this.connectionHandler.onDisconnected(this, ex.getMessage());
			this.connected = false;
			this.connectionHandler = null;
			this.socket = null;
		}
	};
	
	private void handleLights()
	{
		turnLightsBlinkWaited += 10;
		
		if (turnLightsBlinkWaited > 2000)
		{
			turnLightsBlinkWaited = 0;
			
			setLight(leftTurnLightId, false, lastLeftTurnOn);
			lastLeftTurnOn = false;

			setLight(rightTurnLightId, false, lastRightTurnOn);
			lastRightTurnOn = false;
		}
		else if (turnLightsBlinkWaited > 1000)
		{
			if (true == leftTurnOn)
			{
				setLight(leftTurnLightId, true, lastLeftTurnOn);
				lastLeftTurnOn = true;
			}
			else
			{
				setLight(leftTurnLightId, false, lastLeftTurnOn);
				lastLeftTurnOn = false;
			}

			if (true == this.rightTurnOn)
			{
				setLight(rightTurnLightId, true, this.lastRightTurnOn);
				this.lastRightTurnOn = true;
			}
			else
			{
				setLight(rightTurnLightId, false, this.lastRightTurnOn);
				this.lastRightTurnOn = false;
			}
		}
		
		this.reverseLightOn = this.throttle < this.centerThrottle.getValueInt();
		
		setLight(reverseLightId, this.reverseLightOn, this.lastReverseLightOn);
		
		this.lastReverseLightOn = this.reverseLightOn;
	};

	private void executeCommands()
	{
		Runnable command = this.commandsQueue.poll();

		if (null != command)
		{
			command.run();
		}
	};
	
	private void sleep(long time)
	{
		try
		{
			Thread.sleep(time);
		}
		catch (InterruptedException e)
		{
			e.printStackTrace();
		}		
	};

	private void connectToWiFi(String bsid, String ssid, Activity activity)
	{
		new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				sleep(60000);

				if (false == WifiCarController.this.connected)
				{
					failConnection("connection time-out");
				}
			}
		}).start();

		WifiConfiguration conf = new WifiConfiguration();

		conf.SSID = "\"" + ssid + "\"";
		conf.BSSID = bsid;
		conf.preSharedKey = "\"" + WifiCarController.networkPass + "\"";

		WifiManager wifiManager = (WifiManager) activity.getSystemService(Context.WIFI_SERVICE);
		int id = wifiManager.addNetwork(conf);
		
		if (id != -1)
		{
			if (false == wifiManager.enableNetwork(id, true))
			{
				failConnection("failure to enable wifi network");
			}
		}
		else
		{
			failConnection("failure to add wifi network");
		}
	};

	private void failConnection(String message)
	{
		if (null != this.connectionHandler)
		{
			this.connectionHandler.onDisconnected(this, message);
		}
	};
	
	@Override
	public void disconnect()
	{
		WifiManager wifiManager = (WifiManager) this.activity.getSystemService(Context.WIFI_SERVICE);

		wifiManager.disconnect();

		closeConnection();
	};

	private synchronized void closeConnection()
	{
		this.connected = false;

		if (null != this.connectionHandler)
		{
			this.connectionHandler.onDisconnected(this, null);
			this.connectionHandler = null;
		}

		if (null != this.socket)
		{
			this.socket.close();
			this.socket = null;
		}
		
/*		if ((null != this.connectionStateHandler) && (null != this.activity))
		{
			try
			{
				this.activity.unregisterReceiver(this.connectionStateHandler);
			}
			catch (IllegalArgumentException ex)
			{
			}
		}*/
	};

	private void setLight(int light, boolean state, boolean previousState)
	{
		if (state != previousState)
		{
			setLight(light, state);
		}
	};
	
	private void setLight(final int light, final boolean state)
	{
		this.commandsQueue.add(new Runnable()
		{
			@Override
			public void run()
			{
				HttpURLConnection urlConnection = null;

				try
				{
					URL url = new URL("http://" + WifiCarController.defaultIp + "/lights/" + light + "/" + (state ? "on" : "off"));
					urlConnection = (HttpURLConnection) url.openConnection();

					StringBuilder sb = new StringBuilder();
					BufferedReader br = new BufferedReader(new InputStreamReader(new BufferedInputStream(urlConnection.getInputStream())));

					String read;

					while ((read = br.readLine()) != null)
					{
						sb.append(read);
					}

					br.close();
				}
				catch (Exception e)
				{
					e.printStackTrace();
				}
				finally
				{
					if (null != urlConnection)
					{
						urlConnection.disconnect();
					}
				}
			}
		});		
	};
	
	@Override
	public void setMainLights(boolean on)
	{
		setLight(frontLightId, on);
	};

	@Override
	public void setLeftTurnLights(boolean on)
	{
		this.leftTurnOn = on;
	};

	@Override
	public void setRightTurnLights(boolean on)
	{
		this.rightTurnOn = on;
	};

	@Override
	public void setBackLights(boolean on)
	{
		setLight(backLightId, on);
	};

	@Override
	public void setReverseLights(boolean on)
	{
		setLight(reverseLightId, on);
	};

	@Override
	public void setSteering(int value)
	{
		this.steering = value;
	};

	@Override
	public void setThrottle(int value)
	{
		this.throttle = value;
	};

	private static byte[] intToByteArray(int a)
	{
		byte[] ret = new byte[4];

		ret[3] = (byte) (a & 0xFF);
		ret[2] = (byte) ((a >> 8) & 0xFF);
		ret[1] = (byte) ((a >> 16) & 0xFF);
		ret[0] = (byte) ((a >> 24) & 0xFF);

		return ret;
	};

	private synchronized void sendValues()
	{
		if (null == this.socket)
		{
			return;
		}
		
		try
		{
			byte[] message = new byte[4];

			byte[] st = WifiCarController.intToByteArray(this.steering);
			byte[] th = WifiCarController.intToByteArray(this.throttle);

			message[0] = st[2];
			message[1] = st[3];
			message[2] = th[2];
			message[3] = th[3];

			this.socket.send(new DatagramPacket(message, message.length, this.inetAddress, 9999));
		}
		catch (Throwable ex)
		{
		//	Log.e("ddd", ex.getMessage());
		}
	};

	@Override
	public String getName()
	{
		if (null != this.scanResult)
		{
			return this.scanResult.SSID.substring(WifiCarController.wifiPrefix.length()).replace('_', ' ');
		}
		else
		{
			return this.name.getValueString();
		}
	};

	@Override
	public void setName(String name)
	{
		this.name.setValue(name);
	};

	@Override
	public int getMinThrottle()
	{
		return this.minThrottle.getValueInt();
	};

	@Override
	public void setMinThrottle(int minThrottle)
	{
		this.minThrottle.setValue(minThrottle);
	};

	@Override
	public int getMaxThrottle()
	{
		return this.maxThrottle.getValueInt();
	};

	@Override
	public void setMaxThrottle(int maxThrottle)
	{
		this.maxThrottle.setValue(maxThrottle);
	};

	@Override
	public int getCenterThrottle()
	{
		return this.centerThrottle.getValueInt();
	};

	@Override
	public void setCenterThrottle(int centerThrottle)
	{
		this.centerThrottle.setValue(centerThrottle);
	};

	@Override
	public int getMinSteering()
	{
		return this.minSteering.getValueInt();
	};

	@Override
	public void setMinSteering(int minSteering)
	{
		this.maxSteering.setValue(minSteering);
	};

	@Override
	public int getMaxSteering()
	{
		return this.maxSteering.getValueInt();
	};

	@Override
	public void setMaxSteering(int maxSteering)
	{
		this.maxSteering.setValue(maxSteering);
	};

	@Override
	public int getCenterSteering()
	{
		return this.centerSteering.getValueInt();
	};

	@Override
	public void setCenterSteering(int centerSteering)
	{
		this.centerSteering.setValue(centerSteering);
	};

	@Override
	public int getPulseWidth()
	{
		return 20;
	};

	@Override
	public void setPulseWidth(int pulseWidth)
	{
	};

	@Override
	public int getMaxBatteryVoltage()
	{
		return 100;
	};

	@Override
	public void setMaxBatteryVoltage(int maxBatteryVoltage)
	{
	};

	@Override
	public int getBatteryCapacity()
	{
		return 0;
	};

	@Override
	public void setBatteryCapacity(int batteryCapacity)
	{
	};

	@Override
	public int getCurrentUsage()
	{
		return 0;
	};

	@Override
	public int getBatteryVoltage()
	{
		return this.batteryStatus.getValueInt() * 100 / 1024;
	};

	@Override
	public int getCapabilities()
	{
		return 8;
	};

	@Override
	public long getLatency()
	{
		return 0;
	};

	@Override
	public IFirmwareUpdater getUpdater()
	{
		return this.updater;
	};

	@Override
	public int getMaxPosibleSteering()
	{
		return 2000;
	};

	@Override
	public int getMaxPosibleThrottle()
	{
		return 2000;
	};

	Queue<Runnable> commandsQueue = new ConcurrentLinkedQueue<Runnable>();
	private SettingsValue minSteering = new SettingsValue("settings/steering/min", 1000, this);
	private SettingsValue maxSteering = new SettingsValue("settings/steering/max", 2000, this);
	private SettingsValue centerSteering = new SettingsValue("settings/steering/center", 1500, this);
	private SettingsValue minThrottle = new SettingsValue("settings/throttle/min", 1000, this);
	private SettingsValue maxThrottle = new SettingsValue("settings/throttle/max", 2000, this);
	private SettingsValue centerThrottle = new SettingsValue("settings/throttle/center", 1500, this);
	private SettingsValue batteryStatus = new SettingsValue("battery", 1024, this);
	private SettingsValue name = new SettingsValue("settings/name", "Noname", this);
	private int throttle = 0xffff;
	private int steering = this.centerSteering.getValueInt();
	private boolean connected = false;
	private DatagramSocket socket;
	private InetAddress inetAddress;
	private IConnectionHandler connectionHandler;
	private ScanResult scanResult = null;
	private Activity activity = null;
	public final static String wifiPrefix = "MyRC-";
	final static String defaultIp = "192.168.4.1";
	private final static String networkPass = "magicpass123";
	private WiFiFirmwareUpdater updater = new WiFiFirmwareUpdater();
	private boolean leftTurnOn = false;
	private boolean rightTurnOn = false;
	private boolean reverseLightOn = false;
	private int turnLightsBlinkWaited = 0;
	private boolean lastLeftTurnOn = false;
	private boolean lastRightTurnOn = false;
	private boolean lastReverseLightOn = false;
	private final static int leftTurnLightId = 1;
	private final static int rightTurnLightId = 0;
	private final static int frontLightId = 2;
	private final static int backLightId = 3;
	private final static int reverseLightId = 4;
};
