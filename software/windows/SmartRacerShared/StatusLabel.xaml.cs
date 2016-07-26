using System;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace SmartRacerShared
{
	public sealed partial class StatusLabel : UserControl
	{
		public StatusLabel()
		{
			this.InitializeComponent();

			if (true == AppSettings.IsBleZee)
			{
				this.C1.Background = new SolidColorBrush(AppSettings.BleZeeColor);
				this.C2.Background = new SolidColorBrush(AppSettings.BleZeeColor);
			}
		}

		public String Key
		{
			get
			{
				return this.textBlock1.Text;
			}
			set
			{
				this.textBlock1.Text = value;
			}
		}

		public String Value
		{
			get
			{
				return this.textBlock.Text;
			}
			set
			{
				this.textBlock.Text = value;
			}
		}
	}
}
