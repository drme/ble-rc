using Microsoft.Phone.Controls;
using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Navigation;
using System.Windows.Threading;

namespace SmartRacer
{
	public partial class StatusPage : PhoneApplicationPage
	{
		public StatusPage()
		{
			InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.ApplicationBar.BackgroundColor = AppSettings.BleZeeColor;
			}

			UpdateStatus(null, MainPage.Instance.ConnectedCar);
		}

		private float GetPercentage(float value, float max)
		{
			if (max <= 0.0f)
			{
				return 0.0f;
			}
			else
			{
				float percent = value * 100.0f / max;

				if (percent > 100.0f)
				{
					percent = 100.0f;
				}

				return percent;
			}
		}

		private void UpdateStatus(object sender, ICar car)
		{
			Update(() =>
			{
				try
				{
					if (null != car)
					{
						this.mainBatteryVoltage.Text = String.Format("{0:0.0} V", car.Status.BatteryVoltage / 1000.0f);
						this.mainBatteryPercentage.Text = String.Format("{0:0.0} %", GetPercentage(car.Status.BatteryVoltage, car.Settings.MaxBatteryVoltage));
						this.mainBatteryProgress.Value = GetPercentage(car.Status.BatteryVoltage, car.Settings.MaxBatteryVoltage);

						this.cpuVoltage.Text = String.Format("{0:0.0} V", car.Status.ReceiverVoltage / 1000.0f);
						this.cpuVoltagePercentage.Text = String.Format("{0:0.0} %", GetPercentage(car.Status.ReceiverVoltage, 3300));
						this.cpuVoltageProgress.Maximum = 100;
						this.cpuVoltageProgress.Value = GetPercentage(car.Status.ReceiverVoltage, 3300);

						this.currentConsumption.Text = String.Format("{0:0.0} A", car.Status.BatteryCurrent / 1000.0f);
						this.currentConsumptionProgress.Maximum = car.Settings.MaxCurrent;
						this.currentConsumptionProgress.Value = car.Status.BatteryCurrent;

						this.cpuTemperature.Text = String.Format("{0:0.0} C", car.Status.ReceiverTemperature);
						this.cpuTemperatureProgress.Maximum = 50;
						this.cpuTemperatureProgress.Value = car.Status.ReceiverTemperature;

						this.engineTemperature.Text = String.Format("{0:0.0} C", car.Status.EngineTemperature);
						this.engineTemperatureProgress.Maximum = 100;
						this.engineTemperatureProgress.Value = car.Status.EngineTemperature;

						this.acceleration.Text = String.Format("({0:0.0}; {1:0.0}; {2:0.0})", car.Status.Acceleration.X, car.Status.Acceleration.Y, car.Status.Acceleration.Z);
						this.latency.Text = String.Format("{0:0} ms", car.Status.ConnectionLatency);

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

						this.TitleBar.Text = car.Name;
					}
					else
					{
						if (AppSettings.IsBleZee)
						{
							this.TitleBar.Text = "BleZee";
						}
					}
				}
				catch (Exception ex)
				{
					Debug.WriteLine(ex.Message);
				}
			});
		}

		private void StartUpdatingStatus(object sender, RoutedEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.StatusUpdated += this.UpdateStatus;
			}
		}

		private void StopUpdatingStatus(object sender, RoutedEventArgs e)
		{
			if (null != MainPage.Instance.ConnectedCar)
			{
				MainPage.Instance.ConnectedCar.StatusUpdated -= this.UpdateStatus;
			}
		}

		private void Update(UpdateUIHandler handler)
		{
			if (Dispatcher.CheckAccess() == false)
			{
				Dispatcher.BeginInvoke(() => { handler(); });
			}
			else
			{
				handler();
			}
		}

		private void BackClicked(object sender, EventArgs e)
		{
			NavigationService.GoBack();
		}

		private delegate void UpdateUIHandler();
	}
}
