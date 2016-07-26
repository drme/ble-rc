using System;
using System.Collections.Generic;
using Windows.System;
using Windows.UI.ApplicationSettings;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SmartRacerShared
{
	public sealed partial class SelectCarPage : Page
	{
		public SelectCarPage()
		{
			this.InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.bottomAppBar.Background = new SolidColorBrush(AppSettings.BleZeeColor);
			}
		}

		private void GoBack(object sender, RoutedEventArgs e)
		{
			Window.Current.Content = AppSettings.MainPage;
			Window.Current.Activate();
		}

		private void OpenPcSettings(object sender, RoutedEventArgs e)
		{
			#pragma warning disable CS4014
			Launcher.LaunchUriAsync(new Uri("ms-settings-bluetooth:///"));
			#pragma warning restore CS4014
		}

		private void CarItemClicked(object sender, ItemClickEventArgs e)
		{
			((IMainPage)AppSettings.MainPage).ConnectCar((ICar)e.ClickedItem);

			GoBack(sender, null);
		}

		private async void PageLoaded(object sender, RoutedEventArgs e)
		{
			SmartRacerShared.CarConnector connector = new SmartRacerShared.CarConnector();

			var carsList = await connector.FindAvailableCars();

			this.itemGridView.DataContext = carsList;
		}

		private void GetCarsClicked(object sender, RoutedEventArgs e)
		{
			#pragma warning disable CS4014
			Launcher.LaunchUriAsync(new Uri((!AppSettings.IsBleZee) ? "https://github.com/drme/ble-rc/" : "https://www.tindie.com/products/SingletonLabs/ble-for-mini-z/"));
			#pragma warning restore CS4014
		}

		private void AddDemoCarsClicked(object sender, RoutedEventArgs e)
		{
			List<ICar> cars = (List<ICar>)this.itemGridView.DataContext;

			cars.Add(new FakeCar("BMW M3 GT2"));
			cars.Add(new FakeCar("Porsche 911 GT3"));
			cars.Add(new FakeCar("Ferrari 458"));

			this.itemGridView.DataContext = new List<ICar>(cars);
		}
	}
}
