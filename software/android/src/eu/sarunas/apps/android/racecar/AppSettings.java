package eu.sarunas.apps.android.racecar;

import android.annotation.TargetApi;
import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Build;
import android.view.Window;
import android.view.WindowManager;

@TargetApi(21)
class AppSettings
{
	public enum AccelerometerAxis
	{
		X("x"), Y("y"), Z("z");

		AccelerometerAxis(String value)
		{
			this.value = value;
		};

		public static AccelerometerAxis fromString(String value)
		{
			for (AccelerometerAxis axis : AccelerometerAxis.values())
			{
				if (axis.value.equals(value))
				{
					return axis;
				}
			}

			return Y;
		};

		private String value;
	};

	private AppSettings()
	{
	};

	private boolean getSettingsValue(String id, boolean defaultValue)
	{
		if (null != this.mainActivity)
		{
			SharedPreferences sharedPref = this.mainActivity.getPreferences(Context.MODE_PRIVATE);

			return sharedPref.getBoolean(id, defaultValue);
		}

		return defaultValue;
	};

	private String getSettingsValue(String id, String defaultValue)
	{
		if (null != this.mainActivity)
		{
			SharedPreferences sharedPref = this.mainActivity.getPreferences(Context.MODE_PRIVATE);

			return sharedPref.getString(id, defaultValue);
		}

		return defaultValue;
	};

	private void setSettingsValue(String id, boolean value)
	{
		if (null != this.mainActivity)
		{
			SharedPreferences sharedPref = this.mainActivity.getPreferences(Context.MODE_PRIVATE);
			SharedPreferences.Editor editor = sharedPref.edit();
			editor.putBoolean(id, value);
			editor.commit();
		}
	};

	private void setSettingsValue(String id, String value)
	{
		if (null != this.mainActivity)
		{
			SharedPreferences sharedPref = this.mainActivity.getPreferences(Context.MODE_PRIVATE);
			SharedPreferences.Editor editor = sharedPref.edit();
			editor.putString(id, value);
			editor.commit();
		}
	};

	public boolean isUseAccelerometer()
	{
		return this.useAccelerometer;
	};

	public void setUseAccelerometer(boolean useAccelerometer)
	{
		this.useAccelerometer = useAccelerometer;

		setSettingsValue(AppSettings.useAccelerometerKey, useAccelerometer);
	};

	public boolean isInvertSteering()
	{
		return this.invertSteering;
	};

	public void setInvertSteering(boolean invertSteering)
	{
		this.invertSteering = invertSteering;

		setSettingsValue(AppSettings.invertSteeringKey, this.invertSteering);
	};

	public boolean isInvertThrottle()
	{
		return this.invertThrottle;
	};

	public void setInvertThrottle(boolean invertThrottle)
	{
		this.invertThrottle = invertThrottle;

		setSettingsValue(AppSettings.invertThrottleKey, this.invertThrottle);
	};

	public boolean isCenterSteering()
	{
		return this.centerSteering;
	};

	public void setCenterSteering(boolean centerSteering)
	{
		this.centerSteering = centerSteering;

		setSettingsValue(AppSettings.centerSteeringKey, this.centerSteering);
	};

	public AccelerometerAxis getAccelerometerAxis()
	{
		return this.accelerometerAxis;
	};

	public void setAccelerometerAxis(AccelerometerAxis accelerometerAxis)
	{
		this.accelerometerAxis = accelerometerAxis;

		setSettingsValue(AppSettings.accelerometerAxisKey, this.accelerometerAxis.value);
	};

	public void setActivity(Activity activity)
	{
		this.mainActivity = activity;

		this.invertSteering = getSettingsValue(AppSettings.invertSteeringKey, this.invertSteering);
		this.invertThrottle = getSettingsValue(AppSettings.invertThrottleKey, this.invertThrottle);
		this.centerSteering = getSettingsValue(AppSettings.centerSteeringKey, this.centerSteering);
		this.useAccelerometer = getSettingsValue(AppSettings.useAccelerometerKey, this.useAccelerometer);
		this.accelerometerAxis = AccelerometerAxis.fromString(getSettingsValue(AppSettings.accelerometerAxisKey, this.accelerometerAxis.value));
	};

	public static AppSettings getInstance()
	{
		if (null == AppSettings.instance)
		{
			AppSettings.instance = new AppSettings();
		}

		return AppSettings.instance;
	};

	@SuppressWarnings("deprecation")
	public void updateTitle(Activity activity, boolean hasBack)
	{
		ActionBar bar = activity.getActionBar();

		if (null != bar)
		{
			if (true == hasBack)
			{
				bar.setHomeButtonEnabled(true);
				bar.setIcon(R.drawable.abc_ic_ab_back_mtrl_am_alpha);
			}
			else
			{
				bar.setIcon(new ColorDrawable(activity.getResources().getColor(android.R.color.transparent)));
			}
		}

		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)
		{
			Window window = activity.getWindow();
			window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
			window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS);

			int color = Target.current == AppType.SmartRacer ? activity.getResources().getColor(R.color.smart_racer_color) : 0xff658cdd;

			float hsv[] = { 0.0f, 0.0f, 0.0f };

			Color.colorToHSV(color, hsv);
			hsv[2] = 0.5f;

			window.setStatusBarColor(Color.HSVToColor(Color.alpha(color), hsv));
		}
	};

	private Activity mainActivity = null;
	private boolean useAccelerometer = true;
	private boolean invertSteering = false;
	private boolean invertThrottle = false;
	private boolean centerSteering = false;
	private AccelerometerAxis accelerometerAxis = AccelerometerAxis.Y;
	private static AppSettings instance = null;
	private final static String centerSteeringKey = "center_steering";
	private final static String invertThrottleKey = "invert_throttle";
	private final static String invertSteeringKey = "invert_steering";
	private final static String useAccelerometerKey = "use_accelerometer";
	private final static String accelerometerAxisKey = "accelerometer_axis";
};
