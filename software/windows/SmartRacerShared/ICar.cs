using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Windows.Foundation;

namespace SmartRacerShared
{
	public sealed class ChannelLimits
	{
		private UInt16 Max = 500;

		public UInt16 Max1
		{
			get { return Max; }
			set { Max = value; }
		}
		private UInt16 Min = 250;

		public UInt16 Min1
		{
			get { return Min; }
			set { Min = value; }
		}
		private UInt16 Center = 375;

		public UInt16 Center1
		{
			get { return Center; }
			set { Center = value; }
		}
	}

	abstract class Settings : SmartRacerShared.ISettings
	{
		protected Settings()
		{
			this.MaxBatteryVoltage = 7400;
			this.MaxCurrent = 30000;
			this.MinBatteryVoltage = 6000;
			this.BatteryCapacity = 5000;
		}

		private ChannelLimits ThrottleSettings = new ChannelLimits();

		public ChannelLimits ThrottleSettings1
		{
			get { return ThrottleSettings; }
			set { ThrottleSettings = value; }
		}
		private ChannelLimits SteeringSettings = new ChannelLimits();

		public ChannelLimits SteeringSettings1
		{
			get { return SteeringSettings; }
			set { SteeringSettings = value; }
		}
		private UInt16 BatteryCapacity;

		public UInt16 BatteryCapacity1
		{
			get { return BatteryCapacity; }
			set { BatteryCapacity = value; }
		}
		private UInt16 MaxBatteryVoltage;

		public UInt16 MaxBatteryVoltage1
		{
			get { return MaxBatteryVoltage; }
			set { MaxBatteryVoltage = value; }
		}
		private String CarName = "no-name";

		public String CarName1
		{
			get { return CarName; }
			set { CarName = value; }
		}
		private UInt16 MaxCurrent;

		public UInt16 MaxCurrent1
		{
			get { return MaxCurrent; }
			set { MaxCurrent = value; }
		}
		private UInt16 MinBatteryVoltage;

		public UInt16 MinBatteryVoltage1
		{
			get { return MinBatteryVoltage; }
			set { MinBatteryVoltage = value; }
		}
		private LightsSettings LightsSettings = new LightsSettings();

		public LightsSettings LightsSettings1
		{
			get { return LightsSettings; }
			set { LightsSettings = value; }
		}
		private UInt16 Capabilities;

		public UInt16 Capabilities1
		{
			get { return Capabilities; }
			set { Capabilities = value; }
		}

		public int MaxSteeringValue
		{
			get
			{
				if ((this.Capabilities & 16) > 0)
				{
					return 2000;
				}
				else
				{
					return 700;
				}
			}
		}

		public abstract void Save();
	}

	public struct Vector3D
	{
		public float X;
		public float Y;
		public float Z;
	}

	public sealed class Status
	{
		public float BatteryVoltage { get; set; }
		public float BatteryCurrent { get; set; }
		public float ReceiverVoltage { get; set; }
		public float ReceiverTemperature { get; set; }
		public float EngineTemperature { get; set; }
		public float ConnectionLatency { get; set; }
		public float SignalStrength { get; set; }
		public Vector3D Acceleration { get; set; }
	}

	public sealed class LightsSettings
	{
		public int MainLights { get; set; }
		public int BackLights { get; set; }
		public int LeftTurnLights { get; set; }
		public int RightTurnLights { get; set; }
		public int ReverseLights { get; set; }
	}

	public interface ILights
	{
		bool MainLightsOn { set; get; }
		bool BackLightsOn { set; get; }
		bool LeftTurnLightsOn { set; get; }
		bool RightTurnLightsOn { set; get; }
		bool ReverseLightsOn { set; get; }
	}

	public interface ICar
	{
		float Throttle { set; get; }
		float Steering { set; get; }
		ILights Lights { get; }
		Status Status { get; }
		ISettings Settings { get; }
		void Connect();
		void Disconnect();
		void TryThrottle(UInt16 value);
		void TrySteering(UInt16 value);

		void BeginSetup();
		void EndSettup();

		String Name { get; }
		String Address { get; }

		event EventHandler<ICar> Connected;
		event EventHandler<ICar> Disconnected;
		event EventHandler<ICar> StatusUpdated;
	}

	public interface ICarFactory
	{
		IAsyncOperation<IEnumerable<ICar>> FindAvailableCars();
	}

	public sealed class CarConnector : ICarFactory
	{
		public CarConnector()
		{
			AddFactory(new BluetoothLECarFactory());
		}

		public void AddFactory(ICarFactory factory)
		{
			this.factories.Add(factory);
		}

		private async Task<IEnumerable<ICar>> FindAvailableCarsAsync()
		{
			List<ICar> cars = new List<ICar>();

			foreach (ICarFactory factory in this.factories)
			{
				cars.AddRange(await factory.FindAvailableCars());
			}

			if (cars.Count <= 0)
			{
				cars.Add(new FakeCar());
			}

			return cars;
		}

		public IAsyncOperation<IEnumerable<ICar>> FindAvailableCars()
		{
			return FindAvailableCarsAsync().AsAsyncOperation();
		}

		public static ICar ActiveCar
		{
			get;
			set;
		}

		private List<ICarFactory> factories = new List<ICarFactory>();
	}
}
