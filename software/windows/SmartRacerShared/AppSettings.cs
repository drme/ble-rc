using System;
using System.Collections.Generic;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Xaml;

namespace SmartRacerShared
{
	public enum SteeringType : int
	{
		Keyboard = 0,
		GamePad = 1,
		TouchScreen = 2,
		Accelerometer = 3
	}

	public sealed class AppSettings
	{
		public AppSettings()
		{
			this.InvertSteering = GetValueOrDefault<bool>(this.invertSteeringKey, false);
			this.InvertThrottle = GetValueOrDefault<bool>(this.invertThrottleKey, false);
			this.ControllType = (SteeringType)GetValueOrDefault<int>(this.controllKey, (int)SteeringType.GamePad);
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

		public SteeringType ControllType
		{
			get;
			set;
		}

		private void AddOrUpdateValue(String key, Object value)
		{
			if (ApplicationData.Current.LocalSettings.Values.ContainsKey(key))
			{
				ApplicationData.Current.LocalSettings.Values[key] = value;
			}
			else
			{
				ApplicationData.Current.LocalSettings.Values.Add(key, value);
            }
		}

		private T GetValueOrDefault<T>(String key, T defaultValue)
		{
			T value;

			try
			{
				if (ApplicationData.Current.LocalSettings.Values.ContainsKey(key))
				{
					value = (T)ApplicationData.Current.LocalSettings.Values[key];
				}
				else
				{
					value = defaultValue;
				}
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
			AddOrUpdateValue(this.controllKey, (int)this.ControllType);
		}

        public static bool IsBleZee
        {
            get;
            set;
        }

        public static Color BleZeeColor
        {
            get
            {
                return Color.FromArgb(255, 103, 141, 222);
            }
        }

		public static UIElement MainPage
		{
			get;
			set;
		}

		public static AppSettings Instance
		{
			get;
			set;
		}

        private String invertSteeringKey = "invertSteering";
		private String invertThrottleKey = "invertThrottle";
		private String controllKey = "controllType";
	}
}
