using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace SmartRacerShared
{
	public sealed partial class SettingsPage : Page
	{
		public SettingsPage()
		{
			InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.bottomAppBar.Background = new SolidColorBrush(AppSettings.BleZeeColor);
			}

			LoadState();
		}

		private void CancelClicked(object sender, RoutedEventArgs e)
		{
			Window.Current.Content = AppSettings.MainPage;
			Window.Current.Activate();

			ICar car = CarConnector.ActiveCar;

			if (null != car)
			{
				car.EndSettup();
			}
		}

		private void SaveClicked(object sender, RoutedEventArgs e)
		{
			ICar car = CarConnector.ActiveCar;

			if (null != car)
			{
				car.Settings.SteeringSettings1.Min1 = (ushort)this.steeringLeftSlider.Value;
				car.Settings.SteeringSettings1.Max1 = (ushort)this.steeringRightSlider.Value;
				car.Settings.SteeringSettings1.Center1 = (ushort)this.steeringCenterSlider.Value;

				car.Settings.ThrottleSettings1.Center1 = (ushort)this.throttleCenterSlider.Value;
				car.Settings.ThrottleSettings1.Max1 = (ushort)this.throttleForwardSlider.Value;
				car.Settings.ThrottleSettings1.Min1 = (ushort)this.throttleReverseSlider.Value;

				car.Settings.MaxBatteryVoltage1 = ushort.Parse(this.batteryVoltageTextBox.Text);
				car.Settings.BatteryCapacity1 = ushort.Parse(this.batteryCapacityTextBox.Text);
				car.Settings.MaxCurrent1 = ushort.Parse(this.maxCurrentTextBox.Text);
				car.Settings.MinBatteryVoltage1 = ushort.Parse(this.minBatteryVoltageTextBox.Text);

				car.Settings.CarName1 = this.carNameTextBox.Text;

				car.Settings.Save();

				car.EndSettup();
			}

			AppSettings.Instance.InvertSteering = this.invertSteeringCheckBox.IsChecked.Value;
			AppSettings.Instance.InvertThrottle = this.invertThrottleCheckBox.IsChecked.Value;

			if (true == this.useAccelerometer.IsChecked.Value)
			{
				AppSettings.Instance.ControllType = SmartRacerShared.SteeringType.Accelerometer;
			}
			else if (true == this.useKeyboard.IsChecked.Value)
			{
				AppSettings.Instance.ControllType = SmartRacerShared.SteeringType.Keyboard;
			}
			else if (true == this.useTouchscreen.IsChecked.Value)
			{
				AppSettings.Instance.ControllType = SmartRacerShared.SteeringType.TouchScreen;
			}
			else
			{
				AppSettings.Instance.ControllType = SmartRacerShared.SteeringType.GamePad;
			}

			AppSettings.Instance.Save();

			((IMainPage)AppSettings.MainPage).UpdateInfo();

			CancelClicked(sender, e);
		}

		private void FinishSteeringTry(object sender, ManipulationCompletedRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TrySteering((ushort)this.steeringCenterSlider.Value);
			}
		}

		private void TrySteeringLeft(object sender, ManipulationDeltaRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TrySteering((ushort)this.steeringLeftSlider.Value);
			}
		}

		private void TrySteeringRight(object sender, ManipulationDeltaRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TrySteering((ushort)this.steeringRightSlider.Value);
			}
		}

		private void TrySteeringCenter(object sender, ManipulationDeltaRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TrySteering((ushort)this.steeringCenterSlider.Value);
			}
		}

		private void FinishThrottleTry(object sender, ManipulationCompletedRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TryThrottle((ushort)this.throttleCenterSlider.Value);
			}
		}

		private void TryThrottleReverse(object sender, ManipulationDeltaRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TryThrottle((ushort)this.throttleReverseSlider.Value);
			}
		}

		private void TryThrottleForward(object sender, ManipulationDeltaRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TryThrottle((ushort)this.throttleForwardSlider.Value);
			}
		}

		private void TryThrottleCenter(object sender, ManipulationDeltaRoutedEventArgs e)
		{
			if (null != CarConnector.ActiveCar)
			{
				CarConnector.ActiveCar.TryThrottle((ushort)this.throttleCenterSlider.Value);
			}
		}

		private void LoadState()
		{
			ICar car = CarConnector.ActiveCar;

			if (null != car)
			{
				this.steeringLeftSlider.Value = car.Settings.SteeringSettings1.Min1;
				this.steeringRightSlider.Value = car.Settings.SteeringSettings1.Max1;
				this.steeringCenterSlider.Value = car.Settings.SteeringSettings1.Center1;

				this.throttleCenterSlider.Value = car.Settings.ThrottleSettings1.Center1;
				this.throttleForwardSlider.Value = car.Settings.ThrottleSettings1.Max1;
				this.throttleReverseSlider.Value = car.Settings.ThrottleSettings1.Min1;

				this.batteryVoltageTextBox.Text = "" + car.Settings.MaxBatteryVoltage1;
				this.batteryCapacityTextBox.Text = "" + car.Settings.BatteryCapacity1;
				this.maxCurrentTextBox.Text = "" + car.Settings.MaxCurrent1;
				this.minBatteryVoltageTextBox.Text = "" + car.Settings.MinBatteryVoltage1;

				this.carNameTextBox.Text = car.Settings.CarName1;

				this.steeringLeftSlider.IsEnabled = true;
				this.steeringRightSlider.IsEnabled = true;
				this.steeringCenterSlider.IsEnabled = true;

				this.throttleCenterSlider.IsEnabled = true;
				this.throttleForwardSlider.IsEnabled = true;
				this.throttleReverseSlider.IsEnabled = true;

				this.steeringCenterSlider.Maximum = car.Settings.MaxSteeringValue;
				this.steeringLeftSlider.Maximum = car.Settings.MaxSteeringValue;
				this.steeringRightSlider.Maximum = car.Settings.MaxSteeringValue;

				this.pageTitle.Text = (car.Name + " settings").Trim();

				car.BeginSetup();
			}
			else
			{
				this.steeringLeftSlider.IsEnabled = false;
				this.steeringRightSlider.IsEnabled = false;
				this.steeringCenterSlider.IsEnabled = false;

				this.throttleCenterSlider.IsEnabled = false;
				this.throttleForwardSlider.IsEnabled = false;
				this.throttleReverseSlider.IsEnabled = false;
			}

			this.invertSteeringCheckBox.IsChecked = AppSettings.Instance.InvertSteering;
			this.invertThrottleCheckBox.IsChecked = AppSettings.Instance.InvertThrottle;

			switch (AppSettings.Instance.ControllType)
			{
				case SteeringType.Accelerometer:
					this.useAccelerometer.IsChecked = true;
					break;
				case SteeringType.Keyboard:
					this.useKeyboard.IsChecked = true;
					break;
				case SteeringType.TouchScreen:
					this.useTouchscreen.IsChecked = true;
					break;
				case SteeringType.GamePad:
				default:
					this.useGamepad.IsChecked = true;
					break;
			}

			if (AppSettings.IsBleZee)
			{
				this.PowerGroup.Visibility = Visibility.Collapsed;
			}
		}
	}
}
