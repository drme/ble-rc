using Microsoft.Phone.Controls;
using System;
using System.Windows.Navigation;

namespace SmartRacer
{
	public partial class SettingsPage : PhoneApplicationPage
	{
		public SettingsPage()
		{
			InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.ApplicationBar.BackgroundColor = AppSettings.BleZeeColor;
			}

			ICar car = MainPage.Instance.ConnectedCar;

			if (null != car)
			{
				this.steeringLeftSlider.Value = car.Settings.SteeringSettings.Min;
				this.steeringRightSlider.Value = car.Settings.SteeringSettings.Max;
				this.steeringCenterSlider.Value = car.Settings.SteeringSettings.Center;

				this.throttleCenterSlider.Value = car.Settings.ThrottleSettings.Center;
				this.throttleForwardSlider.Value = car.Settings.ThrottleSettings.Max;
				this.throttleReverseSlider.Value = car.Settings.ThrottleSettings.Min;

				this.batteryVoltageTextBox.Text = car.Settings.MaxBatteryVoltage.ToString();
				this.batteryCapacityTextBox.Text = car.Settings.BatteryCapacity.ToString();
				this.maxCurrentTextBox.Text = car.Settings.MaxCurrent.ToString();
				this.minBatteryVoltageTextBox.Text = car.Settings.MinBatteryVoltage.ToString();

				this.carNameTextBox.Text = car.Settings.CarName;

				this.steeringLeftSlider.IsEnabled = true;
				this.steeringRightSlider.IsEnabled = true;
				this.steeringCenterSlider.IsEnabled = true;

				this.throttleCenterSlider.IsEnabled = true;
				this.throttleForwardSlider.IsEnabled = true;
				this.throttleReverseSlider.IsEnabled = true;

				this.steeringCenterSlider.Maximum = car.Settings.MaxSteeringValue;
				this.steeringLeftSlider.Maximum = car.Settings.MaxSteeringValue;
				this.steeringRightSlider.Maximum = car.Settings.MaxSteeringValue;

				car.BeginSetup();

				this.pages.Title = car.Name;
			}
			else
			{
				this.steeringLeftSlider.IsEnabled = false;
				this.steeringRightSlider.IsEnabled = false;
				this.steeringCenterSlider.IsEnabled = false;

				this.throttleCenterSlider.IsEnabled = false;
				this.throttleForwardSlider.IsEnabled = false;
				this.throttleReverseSlider.IsEnabled = false;

				this.pages.Items.Remove(this.steeringPage);
				this.pages.Items.Remove(this.throttlePage);

				this.Title = AppSettings.IsBleZee ? "BleZee" : "Smart Racer";
			}

			this.invertSteeringCheckBox.IsChecked = MainPage.Instance.AppSettings.InvertSteering;
			this.invertThrottleCheckBox.IsChecked = MainPage.Instance.AppSettings.InvertThrottle;
			this.useAccelerometerCheckBox.IsChecked = MainPage.Instance.AppSettings.UseAccelerometer;

			//this.carNameTextBox.IsReadOnly = true;

			if (AppSettings.IsBleZee)
			{
				this.pages.Items.RemoveAt(this.pages.Items.Count - 1);
			}
		}

		private void SaveSettingsClicked(object sender, EventArgs e)
		{
			ICar car = MainPage.Instance.ConnectedCar;

			if (null != car)
			{
				car.Settings.SteeringSettings.Min = (ushort)this.steeringLeftSlider.Value;
				car.Settings.SteeringSettings.Max = (ushort)this.steeringRightSlider.Value;
				car.Settings.SteeringSettings.Center = (ushort)this.steeringCenterSlider.Value;

				car.Settings.ThrottleSettings.Center = (ushort)this.throttleCenterSlider.Value;
				car.Settings.ThrottleSettings.Max = (ushort)this.throttleForwardSlider.Value;
				car.Settings.ThrottleSettings.Min = (ushort)this.throttleReverseSlider.Value;

				car.Settings.MaxBatteryVoltage = ushort.Parse(this.batteryVoltageTextBox.Text);
				car.Settings.BatteryCapacity = ushort.Parse(this.batteryCapacityTextBox.Text);
				car.Settings.MaxCurrent = ushort.Parse(this.maxCurrentTextBox.Text);
				car.Settings.MinBatteryVoltage = ushort.Parse(this.minBatteryVoltageTextBox.Text);

				car.Settings.CarName = this.carNameTextBox.Text;

				car.Settings.Save();

				car.EndSettup();
			}

			MainPage.Instance.AppSettings.InvertSteering = (bool)this.invertSteeringCheckBox.IsChecked;
			MainPage.Instance.AppSettings.InvertThrottle = (bool)this.invertThrottleCheckBox.IsChecked;
			MainPage.Instance.AppSettings.UseAccelerometer = (bool)this.useAccelerometerCheckBox.IsChecked;
			MainPage.Instance.AppSettings.Save();

			NavigationService.GoBack();
		}

		private void CanceButtonClicked(object sender, EventArgs e)
		{
			NavigationService.GoBack();

			ICar car = MainPage.Instance.ConnectedCar;

			if (null != car)
			{
				car.EndSettup();
			}
		}

		private void TryThrottleCenter(object sender, System.Windows.Input.ManipulationDeltaEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TryThrottle((ushort)this.throttleCenterSlider.Value);
			}
		}

		private void TryThrottleForward(object sender, System.Windows.Input.ManipulationDeltaEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TryThrottle((ushort)this.throttleForwardSlider.Value);
			}
		}

		private void TryThrottleReverse(object sender, System.Windows.Input.ManipulationDeltaEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TryThrottle((ushort)this.throttleReverseSlider.Value);
			}
		}

		private void FinishThrottleTry(object sender, System.Windows.Input.ManipulationCompletedEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TryThrottle((ushort)this.throttleCenterSlider.Value);
			}
		}

		private void TrySteeringLeft(object sender, System.Windows.Input.ManipulationDeltaEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TrySteering((ushort)this.steeringLeftSlider.Value);
			}
		}

		private void FinishSteeringTry(object sender, System.Windows.Input.ManipulationCompletedEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TrySteering((ushort)this.steeringCenterSlider.Value);
			}
		}

		private void TrySteeringRight(object sender, System.Windows.Input.ManipulationDeltaEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TrySteering((ushort)this.steeringRightSlider.Value);
			}
		}

		private void TrySteeringCenter(object sender, System.Windows.Input.ManipulationDeltaEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.TrySteering((ushort)this.steeringCenterSlider.Value);
			}
		}
	}
}
