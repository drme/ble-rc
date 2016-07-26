using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartRacerShared
{
	public sealed class Utils
	{
		public static String Format(String format, params Object[] data)
		{
			return String.Format(format, data);
		}

		public static float GetPercentage(float value1, float max)
		{
			if (max <= 0.0f)
			{
				return 0.0f;
			}
			else
			{
				float percent = value1 * 100.0f / max;

				if (percent > 100.0f)
				{
					percent = 100.0f;
				}

				return percent;
			}
		}
	}
}
