using Microsoft.Devices.Sensors;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using SmartRacer.Shared;
using SmartRacerShared;
using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Threading;

namespace SmartRacer
{
	public partial class MainPage : PhoneApplicationPage
	{
		public MainPage()
		{
			InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.ApplicationBar.BackgroundColor = AppSettings.BleZeeColor;
			}

			CarDisconnected(null, null);

			instance = this;

			//this.connector.AddFactory(new BluetoothCarFactory());
			this.connector.AddFactory(new BluetoothLECarFactory());
			//this.connector.AddFactory(new WiFiDirectCarFactory());

			if (true == Accelerometer.IsSupported)
			{
				this.accelerometer.TimeBetweenUpdates = TimeSpan.FromMilliseconds(20);
				this.accelerometer.CurrentValueChanged += AccelerometerValueChanged;
			}

			this.btOkImage = new BitmapImage(new Uri("/Assets/bt-ok.png", UriKind.Relative));
			this.btOffImage = new BitmapImage(new Uri("/Assets/bt-off.png", UriKind.Relative));
			this.btSlowImage = new BitmapImage(new Uri("/Assets/bt-slow.png", UriKind.Relative));
		}

		private void NavigatedToPage(object sender, NavigationEventArgs e)
		{
			this.active = (e.Uri.Equals(new Uri("/SmartRacer.Shared;component/MainPage.xaml", UriKind.Relative)));
		}

		private void UpdateStatus(object sender, ICar e)
		{
			Update(() => 
			{
				try
				{
					if (null != this.ConnectedCar)
					{
						this.connectionStatus.Visibility = Visibility.Visible;

						if (this.connectedCar.Status.ConnectionLatency <= 0.001)
						{
							this.connectionStatus.Source = this.btOffImage;
						}
						else if (this.ConnectedCar.Status.ConnectionLatency < 35)
						{
							this.connectionStatus.Source = this.btOkImage;
						}
						else if (this.ConnectedCar.Status.ConnectionLatency < 75)
						{
							this.connectionStatus.Source = this.btOkImage;
						}
						else
						{
							this.connectionStatus.Source = this.btOffImage;
						}

						//this.speedGage.Maximum = 50000;//this.ConnectedCar.Settings.MaxCurrent;
						//this.speedGage.Value = this.ConnectedCar.Status.BatteryCurrent;

						//this.engineTempGage.Maximum = 100;
						//this.engineTempGage.Value = this.ConnectedCar.Status.EngineTemperature;

						//this.cpuTempGage.Maximum = 100;
						//this.cpuTempGage.Value = this.ConnectedCar.Status.ReceiverTemperature;

						//this.batteryGage.Maximum = this.ConnectedCar.Settings.MaxBatteryVoltage;
						//this.batteryGage.Value = this.ConnectedCar.Status.BatteryVoltage;

						//this.rpmGage.Maximum = 50000;// this.ConnectedCar.Settings.MaxCurrent;
						//this.rpmGage.Value = this.ConnectedCar.Status.BatteryCurrent;
					}
					else
					{
						this.connectionStatus.Visibility = Visibility.Collapsed;
					}
				}
				catch (Exception ex)
				{
					Debug.WriteLine(ex.Message);
				}
			});
		}

		private void AccelerometerValueChanged(object sender, SensorReadingEventArgs<AccelerometerReading> e)
		{
			if (true == this.active)
			{
				if (/*(Visibility.Visible == this.Visibility) && */(true == this.AppSettings.UseAccelerometer))
				{
					float v = e.SensorReading.Acceleration.Y * 2.0f;

					if (v < -1.0f)
					{
						v = -1.0f;
					}

					if (v > 1.0f)
					{
						v = 1.0f;
					}

					if ((v > 0.0f) && (v < 0.1f))
					{
						v = 0.0f;
					}

					if ((v < 0.0f) && (v > -0.1f))
					{
						v = 0.0f;
					}

					if (true == this.AppSettings.InvertSteering)
					{
						v *= -1.0f;
					}

					if (null != this.ConnectedCar)
					{
						this.ConnectedCar.Steering = v;
					}
				}
			}
		}
	
		private void ThrottleChanged(ThumbStick sender, float position)
		{
			if (null != this.connectedCar)
			{
				if (true == this.AppSettings.InvertThrottle)
				{
					this.connectedCar.Throttle = -position;
				}
				else
				{
					this.connectedCar.Throttle = position;
				}
			}
		}

		private void SteeringChanged(ThumbStick sender, float position)
		{
			if ((false == Accelerometer.IsSupported) || (false == AppSettings.UseAccelerometer))
			{
				if (null != this.connectedCar)
				{
					if (true == this.AppSettings.InvertSteering)
					{
						this.connectedCar.Steering = -position;
					}
					else
					{
						this.connectedCar.Steering = position;
					}
				}
			}
		}

		private void PageLoaded(object sender, RoutedEventArgs e)
		{
			this.NavigationService.Navigated += NavigatedToPage;

			if (null != this.ConnectedCar)
			{
				CarConnected(null, null);
			}
			else
			{
				CarDisconnected(null, null);
			}
        }

        private void CarDisconnected(object sender, ICar e)
        {
			Update(() =>
			{
				//this.steeringSlider.Visibility = Visibility.Collapsed;
				//this.throttleSlider.Visibility = Visibility.Collapsed;
				this.throttleSlider.Opacity = 0.3;
				this.steeringSlider.Opacity = 0.3;
				this.throttleSlider.IsEnabled = false;
				this.steeringSlider.IsEnabled = false;

				((ApplicationBarIconButton)ApplicationBar.Buttons[0]).IsEnabled = true;
				((ApplicationBarIconButton)ApplicationBar.Buttons[1]).IsEnabled = false;
				((ApplicationBarIconButton)ApplicationBar.Buttons[2]).IsEnabled = false;
				((ApplicationBarIconButton)ApplicationBar.Buttons[3]).IsEnabled = false;

				//this.speedGage.Visibility = Visibility.Collapsed;
				//this.rpmGage.Visibility = Visibility.Collapsed;
				//this.batteryGage.Visibility = Visibility.Collapsed;
				//this.engineTempGage.Visibility = Visibility.Collapsed;
				//this.cpuTempGage.Visibility = Visibility.Collapsed;
			});

			this.accelerometer.Stop();

			this.ConnectedCar = null;
		}

		private void CarConnected(object sender, ICar e)
		{
			Update(() =>
			{
				//this.steeringSlider.Visibility = System.Windows.Visibility.Visible;
				//this.throttleSlider.Visibility = System.Windows.Visibility.Visible;
				this.throttleSlider.Opacity = 1.0;
				this.steeringSlider.Opacity = ((AppSettings.UseAccelerometer) && (Accelerometer.IsSupported)) ? 0.3 : 1.0;
				this.throttleSlider.IsEnabled = true;
				this.steeringSlider.IsEnabled = ((!AppSettings.UseAccelerometer) || (!Accelerometer.IsSupported));

				((ApplicationBarIconButton)ApplicationBar.Buttons[0]).IsEnabled = false;
				((ApplicationBarIconButton)ApplicationBar.Buttons[1]).IsEnabled = true;
				((ApplicationBarIconButton)ApplicationBar.Buttons[2]).IsEnabled = true;
				((ApplicationBarIconButton)ApplicationBar.Buttons[3]).IsEnabled = true;

				//this.speedGage.Visibility = Visibility.Visible;
				//this.rpmGage.Visibility = Visibility.Visible;
				//this.batteryGage.Visibility = Visibility.Visible;
				//this.engineTempGage.Visibility = Visibility.Visible;
				//this.cpuTempGage.Visibility = Visibility.Visible;
			});

			if ((true == Accelerometer.IsSupported) && (true == AppSettings.UseAccelerometer))
			{
				this.accelerometer.Start();
			}
			else
			{
				this.accelerometer.Stop();
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

		public CarConnector Connector
		{
			get
			{
				return this.connector;
			}
		}

		public ICar ConnectedCar
		{
			set
			{
				if (null != this.connectedCar)
				{
					this.connectedCar.Connected -= this.CarConnected;
					this.connectedCar.Disconnected -= this.CarDisconnected;
					this.connectedCar.StatusUpdated -= this.UpdateStatus;
				}

				this.connectedCar = value;

				if (null != this.connectedCar)
				{
					this.connectedCar.Connected += this.CarConnected;
					this.connectedCar.Disconnected += this.CarDisconnected;
					this.connectedCar.StatusUpdated += this.UpdateStatus;

					this.connectedCar.Connect();
				}
			}
			get
			{
				return this.connectedCar;
			}
		}

		public AppSettings AppSettings
		{
			get
			{
				return this.appSettings;
			}
		}

		public static MainPage Instance
		{
			get
			{
				return instance;
			}
		}

		private void ConnectButtonClicked(object sender, EventArgs e)
		{
			NavigationService.Navigate(new Uri("/SmartRacer.Shared;component/SelectCarPage.xaml", UriKind.Relative));
		}

		private void DisconnectButtonClicked(object sender, EventArgs e)
		{
			if (null != this.connectedCar)
			{
				this.connectedCar.Disconnect();
			}
		}

		private void SettingsButtonClicked(object sender, EventArgs e)
		{
			NavigationService.Navigate(new Uri("/SmartRacer.Shared;component/SettingsPage.xaml", UriKind.Relative));
		}

		private void StatusButtonClicked(object sender, EventArgs e)
		{
			NavigationService.Navigate(new Uri("/SmartRacer.Shared;component/StatusPage.xaml", UriKind.Relative));
		}

		protected override void OnNavigatedTo(NavigationEventArgs e)
		{
			base.OnNavigatedTo(e);
			ICar car = MainPage.Instance.ConnectedCar;

			if (null != car)
			{
				car.EndSettup();
			}

			Update(() =>
			{
				if (null != this.ConnectedCar)
				{
					this.steeringSlider.Opacity = ((AppSettings.UseAccelerometer) && (Accelerometer.IsSupported)) ? 0.3 : 1.0;
					this.steeringSlider.IsEnabled = ((!AppSettings.UseAccelerometer) || (!Accelerometer.IsSupported));
				}
			});
		}

		private CarConnector connector = new CarConnector();
		private ICar connectedCar = null;
		private AppSettings appSettings = new AppSettings();
		private Accelerometer accelerometer = new Accelerometer();
		private static MainPage instance = null;
		private delegate void UpdateUIHandler();
		private bool active = false;
		private BitmapImage btOkImage = null;
		private BitmapImage btOffImage = null;
		private BitmapImage btSlowImage = null;
	}
}
