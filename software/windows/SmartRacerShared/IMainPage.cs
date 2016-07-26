using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartRacerShared
{
	public interface IMainPage
	{
		void UpdateInfo();
		void ConnectCar(ICar car);
	}
}
