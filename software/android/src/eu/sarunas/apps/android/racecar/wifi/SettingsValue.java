package eu.sarunas.apps.android.racecar.wifi;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;

class SettingsValue
{
	public SettingsValue(String url, int defaultValue, WifiCarController controller)
	{
		this.controller = controller;
		this.defaultValue = defaultValue;
		this.url = url;
	};

	public SettingsValue(String url, String defaultValue, WifiCarController controller)
	{
		this.controller = controller;
		this.defaultValueString = defaultValue;
		this.url = url;
	};

	public void setValue(int value)
	{
		this.value = value;

		setValue(String.valueOf(value));
	}

	public void setValue(final String value)
	{
		this.stringValue = value.replace(' ', '_');

		this.controller.commandsQueue.add(new Runnable()
		{
			@Override
			public void run()
			{
				HttpURLConnection urlConnection = null;

				try
				{
					URL url = new URL("http://" + WifiCarController.defaultIp + "/" + SettingsValue.this.url + "?value=" + URLEncoder.encode(SettingsValue.this.stringValue, "UTF-8"));
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

	public int getValueInt()
	{
		if (null != this.value)
		{
			return this.value;
		}
		else
		{
			return this.defaultValue;
		}
	}

	public void refreshValueInt()
	{
		HttpURLConnection urlConnection = null;

		try
		{
			URL url = new URL("http://" + WifiCarController.defaultIp + "/" + this.url);
			urlConnection = (HttpURLConnection) url.openConnection();

			StringBuilder sb = new StringBuilder();
			BufferedReader br = new BufferedReader(new InputStreamReader(new BufferedInputStream(urlConnection.getInputStream())));

			String read;

			while ((read = br.readLine()) != null)
			{
				sb.append(read);
			}

			br.close();

			this.value = Integer.parseInt(sb.toString().trim());
			
			if (this.value == 0xffff)
			{
				this.value = defaultValue;
			}
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
	};

	public String getValueString()
	{
		if (null != this.stringValue)
		{
			return this.stringValue.replace('_', ' ');
		}
		else
		{
			return this.defaultValueString;
		}
	}

	public void refreshValueString()
	{
		HttpURLConnection urlConnection = null;

		try
		{
			URL url = new URL("http://" + WifiCarController.defaultIp + "/" + this.url);
			urlConnection = (HttpURLConnection) url.openConnection();

			StringBuilder sb = new StringBuilder();
			BufferedReader br = new BufferedReader(new InputStreamReader(new BufferedInputStream(urlConnection.getInputStream())));

			String read;

			while ((read = br.readLine()) != null)
			{
				sb.append(read);
			}

			br.close();

			this.stringValue = sb.toString().trim();
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
	};

	private String url;
	private int defaultValue;
	private String defaultValueString;
	private Integer value = null;
	private String stringValue = null;
	private WifiCarController controller;
};
