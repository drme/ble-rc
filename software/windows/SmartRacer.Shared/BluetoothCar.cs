using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using Windows.Networking.Proximity;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace SmartRacer
{
    enum RaceCarCommands : int
    {
        None = 0,
        Left = 1,
        Right = 2,
        Back = 4,
        Forward = 8,
        Horn = 16,
        HandBrake = 32,
        Off = 64
    }

    class BluetoothCar : ICar, ILights
    {
        public BluetoothCar(PeerInformation device)
        {
            this.device = device;

            Debug.WriteLine("Found BT device: " + device.DisplayName + " " + device.HostName + " " + device.ServiceName);
        }

        public float Throttle
        {
            set
            {
                lock (this)
                {
                    if (value > 0)
                    {
                        this.currentState |= RaceCarCommands.Forward;
                        this.currentState &= ~RaceCarCommands.Back;
                    }
                    else if (value < 0)
                    {
                        this.currentState |= RaceCarCommands.Back;
                        this.currentState &= ~RaceCarCommands.Forward;
                    }
                    else
                    {
                        this.currentState &= ~RaceCarCommands.Forward;
                        this.currentState &= ~RaceCarCommands.Back;
                    }
                }
            }
        }

        public float Steering
        {
            set
            {
                lock (this)
                {
                    if (value > 0)
                    {
                        this.currentState |= RaceCarCommands.Right;
                        this.currentState &= ~RaceCarCommands.Left;
                    }
                    else if (value < 0)
                    {
                        this.currentState |= RaceCarCommands.Left;
                        this.currentState &= ~RaceCarCommands.Right;
                    }
                    else
                    {
                        this.currentState &= ~RaceCarCommands.Left;
                        this.currentState &= ~RaceCarCommands.Right;
                    }
                }
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
            get { throw new NotImplementedException(); }
        }

        public Settings Settings
        {
            get { throw new NotImplementedException(); }
        }

        public async void Connect()
        {
            try
            {
                this.socket = new StreamSocket();
				await this.socket.ConnectAsync(this.device.HostName, "1");//{00001101-0000-1000-8000-00805F9B34FB}");
                this.writer = new DataWriter(this.socket.OutputStream);

                this.connected = true;

                Thread thread = new Thread(this.ProcessCommands);
                thread.Start();

                if (null != this.Connected)
                {
                    this.Connected(this, this);
                }
            }
            catch
            {
                if (null != this.Disconnected)
                {
                    this.Disconnected(this, this);
                }
            }
        }

        public void Disconnect()
        {
            this.connected = false;

            this.writer.Dispose();
            this.writer = null;

            this.socket.Dispose();
            this.socket = null;

            if (null != this.Disconnected)
            {
                this.Disconnected(this, this);
            }
        }

		private async void ProcessCommands()
		{
			while (true == this.connected)
			{
				try
				{
					byte data = (byte)this.currentState;

					this.writer.WriteByte(data);

					await this.writer.StoreAsync();
				}
				catch
				{
					this.connected = false;

					if (null != this.Disconnected)
					{
						this.Disconnected(this, this);
					}
				}

				Thread.Sleep(10);
			}
		}

        public bool MainLightsOn
        {
            set
            {
            }
        }

        public bool BackLightsOn
        {
            set
            {
                if (true == value)
                {
                    this.currentState |= RaceCarCommands.HandBrake;
                }
                else
                {
                    this.currentState &= ~RaceCarCommands.HandBrake;
                }
            }
        }

        public bool LeftTurnLightsOn
        {
            set
            {
            }
        }

        public bool RightTurnLightsOn
        {
            set
            {
            }
        }

        public bool ReverseLightsOn
        {
            set
            {
            }
        }

		public String Name
		{
			get
			{
				return this.device.DisplayName;
			}
		}

		public String Address
		{
			get
			{
				return this.device.HostName.RawName;
			}
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
        private PeerInformation device = null;
        private RaceCarCommands currentState = RaceCarCommands.None;
        private StreamSocket socket = null;
        private DataWriter writer = null;
        private bool connected = false;


		public void TryThrottle(ushort value)
		{
			throw new NotImplementedException();
		}

		public void TrySteering(ushort value)
		{
			throw new NotImplementedException();
		}
	}

    class BluetoothCarFactory : ICarFactory
    {
        public async Task<List<ICar>> FindAvailableCars()
        {
            List<ICar> cars = new List<ICar>();

            PeerFinder.AlternateIdentities["Bluetooth:PAIRED"] = "";

            IReadOnlyList<PeerInformation> devices = await PeerFinder.FindAllPeersAsync();

            foreach (PeerInformation device in devices)
            {
                cars.Add(new BluetoothCar(device));
            }

            return cars;
        }
    }
}
