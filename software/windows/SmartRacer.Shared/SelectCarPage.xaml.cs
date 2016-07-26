using Microsoft.Phone.Controls;
using Microsoft.Phone.Tasks;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;

namespace SmartRacer
{
	public partial class SelectCarPage : PhoneApplicationPage
	{
		public SelectCarPage()
		{
			InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.ApplicationBar.BackgroundColor = AppSettings.BleZeeColor;
				this.TitleBox.Text = "BleZee";
			}

			this.DataContext = null;
		}

		private async void PageLoaded(object sender, RoutedEventArgs e)
		{
           List<ICar> cars = await MainPage.Instance.Connector.FindAvailableCars();

		   this.DataContext = cars;
		}

		private void CarSelectionChanged(object sender, SelectionChangedEventArgs e)
		{
			MainPage.Instance.ConnectedCar = (ICar)MainLongListSelector.SelectedItem;

			NavigationService.GoBack();
		}

		private void CancelButtonClicked(object sender, EventArgs e)
		{
			NavigationService.GoBack();
		}

		private void AddFakesButtonClicked(object sender, EventArgs e)
		{
			List<ICar> cars = MainPage.Instance.Connector.GetDemoCars();

			this.DataContext = cars;
		}
		private void GetCarsButtonClicked(object sender, EventArgs e)
		{
			WebBrowserTask task = new WebBrowserTask();
			task.Uri = new Uri((!AppSettings.IsBleZee) ? "https://github.com/drme/ble-rc/" : "https://www.tindie.com/products/SingletonLabs/ble-for-mini-z/");
			task.Show();
		}

		private async void OpenBluetoothSettings(object sender, RoutedEventArgs e)
		{
			await Windows.System.Launcher.LaunchUriAsync(new Uri("ms-settings:bluetooth"));
		}

		private void RefreshButtonClicked(object sender, EventArgs e)
		{
			PageLoaded(sender, null);
		}
	}
}
