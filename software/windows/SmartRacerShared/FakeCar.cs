using System;
using Windows.System.Threading;

namespace SmartRacerShared
{
	class FakeCar : Settings, ICar, ILights
	{
		public FakeCar() : this("Fake demo car")
		{
		}

		public FakeCar(String name)
		{
			this.CarName1 = name;
			this.Name = name;
		}

		private void UpdateStatus(object sender, EventArgs e)
		{
			Random random = new Random();

			this.Status.BatteryVoltage = random.Next(7200);
			this.Status.ReceiverTemperature = random.Next(29);
			this.Status.BatteryCurrent = random.Next(10000);
			this.Status.ReceiverVoltage = 3300;
			this.Status.EngineTemperature = random.Next(70);
			this.Status.ConnectionLatency = random.Next(50);
			NotifyStatusUpdate();
		}

		private void NotifyStatusUpdate()
		{
			if (null != this.StatusUpdated)
			{
				this.StatusUpdated(this, this);
			}
		}

		public float Throttle
		{
			set
			{
			}
			get
			{
				return 0.0f;
			}
		}

		public float Steering
		{
			set
			{
			}
			get
			{
				return 0.0f;
			}
		}

		public ILights Lights
		{
			get
			{
				return this;
			}
		}

		public Status Status
		{
			get
			{
				return this.status;
			}
		}

		public ISettings Settings
		{
			get
			{
				return this;
			}
		}

		public void Connect()
		{
			this.statusUpdateTimer = ThreadPoolTimer.CreatePeriodicTimer((source) => { UpdateStatus(null, null); }, TimeSpan.FromSeconds(1));

			if (null != this.Connected)
			{
				this.Connected(this, this);
			}
		}

		public void Disconnect()
		{
			if (null != statusUpdateTimer)
			{
				this.statusUpdateTimer.Cancel();
				this.statusUpdateTimer = null;
			}

			if (null != this.Disconnected)
			{
				this.Disconnected(this, this);
			}
		}

		public void TryThrottle(ushort value)
		{
		}

		public void TrySteering(ushort value)
		{
		}

		public string Name
		{
			get;
			set;
		}

		public string Address
		{
			get
			{
				return "";
			}
		}

		public bool MainLightsOn
		{
			set { throw new NotImplementedException(); }
		}

		public bool BackLightsOn
		{
			set { throw new NotImplementedException(); }
		}

		public bool LeftTurnLightsOn
		{
			set { throw new NotImplementedException(); }
		}

		public bool RightTurnLightsOn
		{
			set { throw new NotImplementedException(); }
		}

		public bool ReverseLightsOn
		{
			set { throw new NotImplementedException(); }
		}

		public override void Save()
		{
		}

		public void BeginSetup()
		{
		}
		public void EndSettup()
		{
		}

		public event EventHandler<ICar> Connected;
		public event EventHandler<ICar> Disconnected;
		public event EventHandler<ICar> StatusUpdated;

		bool ILights.MainLightsOn
		{
			get
			{
				throw new NotImplementedException();
			}
			set
			{
				throw new NotImplementedException();
			}
		}

		bool ILights.BackLightsOn
		{
			get
			{
				throw new NotImplementedException();
			}
			set
			{
				throw new NotImplementedException();
			}
		}

		bool ILights.LeftTurnLightsOn
		{
			get
			{
				throw new NotImplementedException();
			}
			set
			{
				throw new NotImplementedException();
			}
		}

		bool ILights.RightTurnLightsOn
		{
			get
			{
				throw new NotImplementedException();
			}
			set
			{
				throw new NotImplementedException();
			}
		}

		bool ILights.ReverseLightsOn
		{
			get
			{
				throw new NotImplementedException();
			}
			set
			{
				throw new NotImplementedException();
			}
		}

		private ThreadPoolTimer statusUpdateTimer = null;
		private Status status = new Status();
	}
}
