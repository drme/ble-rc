using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartRacer
{
    public class ChannelLimits
    {
        public UInt16 Max = 500;
        public UInt16 Min = 250;
        public UInt16 Center = 375;
    }

    public abstract class Settings
    {
        public ChannelLimits ThrottleSettings = new ChannelLimits();
        public ChannelLimits SteeringSettings = new ChannelLimits();
        public UInt16 BatteryCapacity;
		public UInt16 MaxBatteryVoltage;
        public String CarName = "no-name";
		public UInt16 MaxCurrent;
		public UInt16 MinBatteryVoltage;
        public LightsSettings LightsSettings = new LightsSettings();
		public UInt16 Capabilities;

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

    public class Status
    {
        public float BatteryVoltage;
        public float BatteryCurrent;
        public float ReceiverVoltage;
        public float ReceiverTemperature;
        public float EngineTemperature;
        public float ConnectionLatency;
        public float SignalStrength;
        public Vector3D Acceleration;
    }

    public class LightsSettings
    {
        public int MainLights;
        public int BackLights;
        public int LeftTurnLights;
        public int RightTurnLights;
        public int ReverseLights;
    }

    public interface ILights
    {
         bool MainLightsOn { set; }
         bool BackLightsOn { set; }
         bool LeftTurnLightsOn { set; }
         bool RightTurnLightsOn { set; }
         bool ReverseLightsOn { set; }
    }

    public interface ICar
    {
        float Throttle { set; }
        float Steering { set; }
        ILights Lights { get; }
        Status Status { get; }
        Settings Settings { get; }
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
         Task< List<ICar> >FindAvailableCars();
    }

    public class CarConnector : ICarFactory
    {
        public void AddFactory(ICarFactory factory)
        {
            this.factories.Add(factory);
        }

        public async Task<List<ICar>> FindAvailableCars()
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

		public List<ICar> GetDemoCars()
		{
			List<ICar> cars = new List<ICar>();

			cars.Add(new FakeCar("BMW M3 GT2"));
			cars.Add(new FakeCar("Porsche 911 GT3"));
			cars.Add(new FakeCar("Ferrari 458"));

			return cars;

		}

		private List<ICarFactory> factories = new List<ICarFactory>();
    }
}
