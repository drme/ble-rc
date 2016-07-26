using System;
using System.Collections.Generic;
using System.IO.IsolatedStorage;
using System.Windows.Media;

namespace SmartRacer
{
	public class AppSettings
	{
		public AppSettings()
		{
			this.isolatedStore = IsolatedStorageSettings.ApplicationSettings;
			this.InvertSteering = GetValueOrDefault<bool>(this.invertSteeringKey, false);
			this.InvertThrottle = GetValueOrDefault<bool>(this.invertThrottleKey, false);
			this.UseAccelerometer = GetValueOrDefault<bool>(this.useAccelerometerKey, true);
		}

		public bool InvertThrottle
		{
			get;
			set;
		}

		public bool InvertSteering
		{
			get;
			set;
		}

		public bool UseAccelerometer
		{
			get;
			set;
		}

		private void AddOrUpdateValue(String key, Object value)
		{
			try
			{
				if (this.isolatedStore[key] != value)
				{
					this.isolatedStore[key] = value;
				}
			}
			catch (KeyNotFoundException)
			{
				this.isolatedStore.Add(key, value);
			}
			catch (ArgumentException)
			{
				this.isolatedStore.Add(key, value);
			}
		}

		private T GetValueOrDefault<T>(String key, T defaultValue)
		{
			T value;

			try
			{
				value = (T)this.isolatedStore[key];
			}
			catch (KeyNotFoundException)
			{
				value = defaultValue;
			}
			catch (ArgumentException)
			{
				value = defaultValue;
			}

			return value;
		}

		public void Save()
		{
			AddOrUpdateValue(this.invertSteeringKey, this.InvertSteering);
			AddOrUpdateValue(this.invertThrottleKey, this.InvertThrottle);
			AddOrUpdateValue(this.useAccelerometerKey, this.UseAccelerometer);
			this.isolatedStore.Save();
		}

		public static bool IsBleZee = false;
		public static Color BleZeeColor = Color.FromArgb(255, 103, 141, 222);

		private String invertSteeringKey = "invertSteering";
		private String invertThrottleKey = "invertThrottle";
		private String useAccelerometerKey = "useAcceleromter";
		private readonly IsolatedStorageSettings isolatedStore;
	}
}
