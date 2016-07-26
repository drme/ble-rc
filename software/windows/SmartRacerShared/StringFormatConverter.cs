using System;
using Windows.UI.Xaml.Data;

namespace SmartRacerShared
{
	public sealed class StringFormatConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string language)
		{
			return string.Format(parameter as string, value);
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			return null;
		}
	}
}
