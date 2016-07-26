using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SmartRacerShared
{
	public sealed partial class StatusPage : Page
	{
		public StatusPage()
		{
			this.InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.bottomAppBar.Background = new SolidColorBrush(AppSettings.BleZeeColor);
			}

			OnStatusUpdated(null, CarConnector.ActiveCar);
		}

		private void CancelClicked(object sender, RoutedEventArgs e)
		{
			Window.Current.Content = AppSettings.MainPage;
			Window.Current.Activate();
		}

		private void CloseClicked(object sender, RoutedEventArgs e)
		{
			Window.Current.Content = AppSettings.MainPage;
			Window.Current.Activate();
		}

		private void StartStatusUpdate(object sender, RoutedEventArgs e)
		{
			var car = CarConnector.ActiveCar;

			if (null != car)
			{
				car.StatusUpdated += this.OnStatusUpdated;
			}
		}

		private String FormatFloat(String format, float value)
		{
			return Utils.Format(format, value);
		}

		private void OnStatusUpdated(object sender, ICar car)
		{
			if (null != car)
			{
				#pragma warning disable CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed
				Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Low, () =>
				{
					this.mainBatteryVoltage.Text = FormatFloat("{0:0.0} V", car.Status.BatteryVoltage / 1000.0f);
					this.mainBatteryPercentage.Text = FormatFloat("{0:0.0} %", Utils.GetPercentage(car.Status.BatteryVoltage, car.Settings.MaxBatteryVoltage1));
					this.mainBatteryProgress.Value = Utils.GetPercentage(car.Status.BatteryVoltage, car.Settings.MaxBatteryVoltage1);

					this.cpuVoltage.Text = FormatFloat("{0:0.0} V", car.Status.ReceiverVoltage / 1000.0f);
					this.cpuVoltagePercentage.Text = FormatFloat("{0:0.0} %", Utils.GetPercentage(car.Status.ReceiverVoltage, 3300));
					this.cpuVoltageProgress.Maximum = 100;
					this.cpuVoltageProgress.Value = Utils.GetPercentage(car.Status.ReceiverVoltage, 3300);

					this.currentConsumption.Text = FormatFloat("{0:0.0} A", car.Status.BatteryCurrent / 1000.0f);
					this.currentConsumptionProgress.Maximum = car.Settings.MaxCurrent1;
					this.currentConsumptionProgress.Value = car.Status.BatteryCurrent;

					this.cpuTemperature.Text = FormatFloat("{0:0.0} C", car.Status.ReceiverTemperature);
					this.cpuTemperatureProgress.Maximum = 50;
					this.cpuTemperatureProgress.Value = car.Status.ReceiverTemperature;

					this.engineTemperature.Text = FormatFloat("{0:0.0} C", car.Status.EngineTemperature);
					this.engineTemperatureProgress.Maximum = 100;
					this.engineTemperatureProgress.Value = car.Status.EngineTemperature;

					this.acceleration.Text = Utils.Format("({0:0.0}; {1:0.0}; {2:0,0})", car.Status.Acceleration.X, car.Status.Acceleration.Y, car.Status.Acceleration.Z);
					this.latency.Text = FormatFloat("{0:0} ms", car.Status.ConnectionLatency);

					if (AppSettings.IsBleZee)
					{
						this.mainBatteryPercentage.Text = "-";
						this.mainBatteryProgress.Value = 0;
						this.mainBatteryVoltage.Text = "";

						this.currentConsumption.Text = "-";
						this.currentConsumptionProgress.Maximum = 100;
						this.currentConsumptionProgress.Value = 0;

						this.engineTemperature.Text = "-";
						this.engineTemperatureProgress.Maximum = 100;
						this.engineTemperatureProgress.Value = 0;

						this.acceleration.Text = "-";
					}

					this.pageTitle.Text = (car.Name + " status").Trim();

				});
				#pragma warning restore CS4014 // Because this call is not awaited, execution of the current method continues before the call is completed
			}
		}

		private void StopStatusUpdate(object sender, RoutedEventArgs e)
		{
			var car = CarConnector.ActiveCar;

			if (null != car)
			{
				car.StatusUpdated -= this.OnStatusUpdated;
			}
		}
	}
}
