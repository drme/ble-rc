using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;
//using System.Windows.Threading;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Devices.Enumeration.Pnp;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.System.Threading;

namespace SmartRacerShared
{
	class BluetoothLECar : Settings, ICar
	{
		public BluetoothLECar(DeviceInformation device)
		{
			this.device = device;
		}

		private async void UpdateStatus(object sender, EventArgs e)
		{
			if (this.stopWatch.IsRunning)
			{
				this.Status.ConnectionLatency = -1;
				NotifyStatusUpdate();
			}

			this.stopWatch.Restart();
			this.stopWatch.Start();
			this.Status.BatteryVoltage = await GetValue(this.batteryVoltageCharacteristics);
			this.Status.ReceiverTemperature = await GetValue(this.mcuTemperatureCharacteristics) / 100.0f;
			this.Status.BatteryCurrent = await GetValue(this.batteryCurrentCharacteristics);
			this.Status.ReceiverVoltage = this.Status.BatteryVoltage > 3300 ? 3300 : this.Status.BatteryVoltage;
			this.Status.EngineTemperature = await GetValue(this.temperatureCharacteristics) / 100.0f;
			this.stopWatch.Stop();
			this.Status.ConnectionLatency = this.stopWatch.ElapsedMilliseconds / 20;
			NotifyStatusUpdate();
		}

		private void NotifyStatusUpdate()
		{
			if (null != this.StatusUpdated)
			{
				this.StatusUpdated(this, this);
			}
		}

		private static async Task<UInt16> GetValue(GattCharacteristic characteristic)
		{
			if ((null != characteristic))
			{
				GattReadResult readResult = await characteristic.ReadValueAsync(BluetoothCacheMode.Uncached);

				if (readResult.Status == GattCommunicationStatus.Success)
				{
					UInt16 d = DataReader.FromBuffer(readResult.Value).ReadUInt16();

					return (UInt16)(((d & 0xff00) >> 8) | (UInt16)(d << 8));
				}
			}

			return 0;
		}

		public float Throttle
		{
			set
			{
				if (true == this.inSetup)
				{
					return;
				}

				float v = value / 2.0f;
    
				float nds = this.Settings.ThrottleSettings1.Center1 - this.Settings.ThrottleSettings1.Min1;
				float pds = this.Settings.ThrottleSettings1.Max1 - this.Settings.ThrottleSettings1.Center1;
    
				ushort vl = (ushort)(this.Settings.ThrottleSettings1.Center1 + ((v < 0) ? (nds * v * 2) : (pds * v * 2)));

				if (vl != this.throttle)
				{
					this.throttle = vl;
				}
				else
				{
					return;
				}

				SetValue(this.throttleCharacteristics, vl);
			}
            get
            {
                return this.throttle;
            }
		}

		public float Steering
		{
			set
			{
				if (true == this.inSetup)
				{
					return;
				}

				float v = value / 2.0f;

				float nds = this.Settings.SteeringSettings1.Center1 - this.Settings.SteeringSettings1.Min1;
				float pds = this.Settings.SteeringSettings1.Max1 - this.Settings.SteeringSettings1.Center1;

				ushort vl = (ushort)(this.Settings.SteeringSettings1.Center1 + ((v < 0) ? (nds * v * 2) : (pds * v * 2)));

				if (vl != this.steereing)
				{
					this.steereing = vl;
				}
				else
				{
					return;
				}

				SetValue(this.steeringCharacteristics, vl);
			}
            get
            {
                return this.steereing;
            }
		}

		private void SetValue(GattCharacteristic characteristic, UInt16 value)
		{
			if (null != characteristic)
			{
				DataWriter writer = new DataWriter();
				UInt16 invertedValue = (UInt16)(value >> 8);
				invertedValue |= (UInt16)(value << 8);
				writer.WriteUInt16(invertedValue);

				//Task.Factory.StartNew(new Action(async () => { await characteristic.WriteValueAsync(writer.DetachBuffer(), GattWriteOption.WriteWithoutResponse); }));
				characteristic.WriteValueAsync(writer.DetachBuffer(), GattWriteOption.WriteWithoutResponse);
			}
		}

		private void SetValue(GattCharacteristic characteristic, String value)
		{
			if (null != characteristic)
			{
				DataWriter writer = new DataWriter();

				byte[] name = new byte[20];

				for (int i = 0; (i < 20) && (i < value.Length); i++)
				{
					name[i] = (byte)value[i];
				}

				writer.WriteBytes(name);

				//Task.Factory.StartNew(new Action(async () => { await characteristic.WriteValueAsync(writer.DetachBuffer(), GattWriteOption.WriteWithoutResponse); }));
				characteristic.WriteValueAsync(writer.DetachBuffer(), GattWriteOption.WriteWithoutResponse);
			}
		}

		public ILights Lights
		{
			get { throw new NotImplementedException(); }
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

		public async void Connect()
		{
			try
			{
				StartDeviceConnectionWatcher();

				this.deviceContainerId = "{" + device.Properties["System.Devices.ContainerId"] + "}";

				this.service = await GattDeviceService.FromIdAsync(device.Id);

				if (this.service != null)
				{
					this.throttleCharacteristics = await GetCharacteristics(0xacc1);
					this.steeringCharacteristics = await GetCharacteristics(0xacc2);
					this.pulseWidthCharacteristics = await GetCharacteristics(0xd101);
					this.nameCharacteristics = await GetCharacteristics(0xf101, (x) => { this.Settings.CarName1 = x; });
					this.minThrottleCharacteristics = await GetCharacteristics(0xb101, (x) => { this.Settings.ThrottleSettings1.Min1 = x; });
					this.maxThrottleCharacteristics = await GetCharacteristics(0xb102, (x) => { this.Settings.ThrottleSettings1.Max1 = x; });
					this.centerThrottleCharacteristics = await GetCharacteristics(0xb103, (x) => { this.Settings.ThrottleSettings1.Center1 = x; });
					this.minSteeringCharacteristics = await GetCharacteristics(0xc101, (x) => { this.Settings.SteeringSettings1.Min1 = x; });
					this.maxSteeringCharacteristics = await GetCharacteristics(0xc102, (x) => { this.Settings.SteeringSettings1.Max1 = x; });
					this.centerSteeringCharacteristics = await GetCharacteristics(0xc103, (x) => { this.Settings.SteeringSettings1.Center1 = x; });

					this.batteryVoltageCharacteristics = await GetCharacteristics(0xe101);
					this.batteryCurrentCharacteristics = await GetCharacteristics(0xe102);

					this.batteryMaxVoltageCharacteristics = await GetCharacteristics(0xe103, (x) => { this.Settings.MaxBatteryVoltage1 = x; });
					this.batteryCapacityCharacteristics = await GetCharacteristics(0xe104, (x) => { this.Settings.BatteryCapacity1 = x; });
					this.batteryMinVoltageCharacteristics = await GetCharacteristics(0xe108, (x) => { this.Settings.MinBatteryVoltage1 = x; });
					this.batteryMaxCurrentCharacteristics = await GetCharacteristics(0xe107, (x) => { this.Settings.MaxCurrent1 = x; });

                    this.deviceCapabilitiesCharacteristic = await GetCharacteristics(0x5001, (x) => { this.Settings.Capabilities1 = x; });

                 //   SetValue(this.deviceCapabilitiesCharacteristic, 16 + 1);

					this.temperatureCharacteristics = await GetCharacteristics(0x3001);
					this.mcuTemperatureCharacteristics = await GetCharacteristics(0x3002);


/*					
					this.controllerBatteryVoltageCharacteristics =await GetCharacteristics(0x2A19);
					this.lightsCharacteristics =await GetCharacteristics(0xa101);
					this.accelerometerEnableCharacteristics =await GetCharacteristics(0xaa12);
					this.accelerometerValueCharacteristics =await GetCharacteristics(0xaa11);
					*/

					this.statusUpdateTimer = ThreadPoolTimer.CreatePeriodicTimer((source) =>
					{
						UpdateStatus(null, null);
					}, TimeSpan.FromSeconds(10));

					this.throttle = this.Settings.ThrottleSettings1.Center1;
					this.steereing = this.Settings.SteeringSettings1.Center1;

					this.commandResender = ThreadPoolTimer.CreatePeriodicTimer((source) =>
					{
						ResendCommands();
					}, TimeSpan.FromMilliseconds(30));


		//			this.commandResender.WorkerSupportsCancellation = true;
		//			this.commandResender.DoWork += this.ResendCommands;
		//			this.commandResender.RunWorkerAsync();

					ResendCommands();

					if (null != this.Connected)
					{
						this.Connected(this, this);
					}
				}
				else
				{
					Debug.WriteLine("Access to the device is denied, because the application was not granted access or the device is currently in use by another application.");

					if (null != this.Disconnected)
					{
						this.Disconnected(this, this);
					}
				}
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.StackTrace.ToString());

				Debug.WriteLine("ERROR: Accessing your device failed." + Environment.NewLine + ex.Message);

				if (null != this.Disconnected)
				{
					this.Disconnected(this, this);
				}
			}
		}

		private void ResendCommands()
		{
			if (null != this.service)
			{
				SetValue(this.throttleCharacteristics, this.throttle);
				SetValue(this.steeringCharacteristics, this.steereing);
			}
		}

		public void BeginSetup()
		{
			this.inSetup = true;
		}
		public void EndSettup()
		{
			this.inSetup = false;
		}

		private async Task<GattCharacteristic> GetCharacteristics(ushort id)
		{
			return await GetCharacteristics(id, (SetValueHandler)null);
		}

		private async Task<GattCharacteristic> GetCharacteristics(ushort id, SetValueHandler setter)
		{
			var characteristic = GetCharacteristicObject(id);

			if ((null != characteristic) && (null != setter))
			{
				GattReadResult readResult = await characteristic.ReadValueAsync();

				if (readResult.Status == GattCommunicationStatus.Success)
				{
					byte[] sensorData = new byte[readResult.Value.Length];
					DataReader.FromBuffer(readResult.Value).ReadBytes(sensorData);

					setter(BitConverter.ToUInt16(sensorData, 0));
				}
			}

			return characteristic;
		}

		private async Task<GattCharacteristic> GetCharacteristics(ushort id, SetStringValueHandler setter)
		{
			var characteristics = GetCharacteristicObject(id);

			if (null != characteristics)
			{
				GattReadResult readResult = await characteristics.ReadValueAsync();

				if (readResult.Status == GattCommunicationStatus.Success)
				{
					byte[] sensorData = new byte[readResult.Value.Length];
					DataReader.FromBuffer(readResult.Value).ReadBytes(sensorData);

					String result = "";

					for (int i = 0; i < sensorData.Length; i++)
					{
						char c = (char)sensorData[i];

						if (c > 0)
						{
							result += c;
						}
					}

					setter(result);
				}
			}

			return characteristics;
		}

		private GattCharacteristic GetCharacteristicObject(ushort id)
		{
			String id128 = "a739" + id.ToString("X4") + "-f6cd-1692-994a-d66d9e0ce048";

			var characteristics = this.service.GetCharacteristics(Guid.Parse(id128));

			if ((null != characteristics) && (characteristics.Count > 0))
			{
				return characteristics[0];
			}
			else
			{
				characteristics = this.service.GetCharacteristics(GattDescriptor.ConvertShortIdToUuid(id));

				if ((null != characteristics) && (characteristics.Count > 0))
				{
					return characteristics[0];
				}
			}

			return null;
		}

		public void Disconnect()
		{
			if (null != this.statusUpdateTimer)
			{
				this.statusUpdateTimer.Cancel();
				this.statusUpdateTimer = null;
			}

			if (null != this.commandResender)
			{
				this.commandResender.Cancel();
				this.commandResender = null;
			}

			if (null != this.service)
			{
				this.service = null;
			}

			if (null != this.Disconnected)
			{
				this.Disconnected(this, this);
			}
		}

		public string Name
		{
			get
			{
				return this.device.Name;
			}
		}

		public string Address
		{
			get
			{
				return "";
			}
		}

		private void StartDeviceConnectionWatcher()
		{
			this.watcher = PnpObject.CreateWatcher(PnpObjectType.DeviceContainer, new string[] { "System.Devices.Connected" }, String.Empty);
			this.watcher.Updated += (PnpObjectWatcher sender, PnpObjectUpdate args) =>
			{
				var connectedProperty = args.Properties["System.Devices.Connected"];

				bool isConnected = false;

				if ((deviceContainerId == args.Id) && Boolean.TryParse(connectedProperty.ToString(), out isConnected) && isConnected)
				{
					this.watcher.Stop();
					this.watcher = null;

					if (true == isConnected)
					{
						if (null != this.Connected)
						{
							this.Connected(this, this);
						}
					}
					else
					{
						if (null != this.Disconnected)
						{
							this.Disconnected(this, this);
						}
					}
				}
			};

			this.watcher.Start();
		}

		public override void Save()
		{
			SetValue(this.minThrottleCharacteristics, this.Settings.ThrottleSettings1.Min1);
			SetValue(this.maxThrottleCharacteristics, this.Settings.ThrottleSettings1.Max1);
			SetValue(this.centerThrottleCharacteristics, this.Settings.ThrottleSettings1.Center1);
			
			SetValue(this.minSteeringCharacteristics, this.Settings.SteeringSettings1.Min1);
			SetValue(this.maxSteeringCharacteristics, this.Settings.SteeringSettings1.Max1);
			SetValue(this.centerSteeringCharacteristics, this.Settings.SteeringSettings1.Center1);

			SetValue(this.batteryMaxVoltageCharacteristics, this.Settings.MaxBatteryVoltage1);
			SetValue(this.batteryCapacityCharacteristics, this.Settings.BatteryCapacity1);
			SetValue(this.batteryMinVoltageCharacteristics, this.Settings.MinBatteryVoltage1);
			SetValue(this.batteryMaxCurrentCharacteristics, this.Settings.MaxCurrent1);

			SetValue(this.nameCharacteristics, this.Settings.CarName1);
		}

		public void TryThrottle(ushort value)
		{
			this.throttle = value;
//			SetValue(this.throttleCharacteristics, value);
		}

		public void TrySteering(ushort value)
		{
			this.steereing = value;
//			SetValue(this.steeringCharacteristics, value);
		}

		public event EventHandler<ICar> Connected;
		public event EventHandler<ICar> Disconnected;
		public event EventHandler<ICar> StatusUpdated;

		private DeviceInformation device = null;
		private GattDeviceService service = null;
		private PnpObjectWatcher watcher = null;
		private String deviceContainerId = null;
		private ushort throttle = 0;
		private ushort steereing = 0;
		private GattCharacteristic throttleCharacteristics = null;
		private GattCharacteristic steeringCharacteristics = null;
		private GattCharacteristic pulseWidthCharacteristics = null;
		private GattCharacteristic nameCharacteristics = null;
		private GattCharacteristic minThrottleCharacteristics = null;
		private GattCharacteristic maxThrottleCharacteristics = null;
		private GattCharacteristic centerThrottleCharacteristics = null;
		private GattCharacteristic minSteeringCharacteristics = null;
		private GattCharacteristic maxSteeringCharacteristics = null;
		private GattCharacteristic centerSteeringCharacteristics = null;
		private GattCharacteristic batteryVoltageCharacteristics = null;
		private GattCharacteristic batteryCurrentCharacteristics = null;
		private GattCharacteristic batteryMaxVoltageCharacteristics = null;
		private GattCharacteristic batteryCapacityCharacteristics = null;
		private GattCharacteristic batteryMinVoltageCharacteristics = null;
		private GattCharacteristic batteryMaxCurrentCharacteristics = null;
		private GattCharacteristic temperatureCharacteristics = null;
		private GattCharacteristic lightsCharacteristics = null;
		private GattCharacteristic accelerometerEnableCharacteristics = null;
		private GattCharacteristic accelerometerValueCharacteristics = null;
		private GattCharacteristic mcuTemperatureCharacteristics = null;
		private GattCharacteristic controllerBatteryVoltageCharacteristics = null;
		private GattCharacteristic deviceCapabilitiesCharacteristic = null;
		private delegate void SetValueHandler(UInt16 value);
		private delegate void SetStringValueHandler(String value);
		private ThreadPoolTimer statusUpdateTimer = null;
		private Status status = new Status();
		private Stopwatch stopWatch = new Stopwatch();
		private ThreadPoolTimer commandResender = null;
		private bool inSetup = false;
	}

	class BluetoothLECarFactory : ICarFactory
	{
		private async Task<IEnumerable<ICar>> FindAvailableCarsAsync()
		{
            List<ICar> cars = new List<ICar>();
            
			var devices = await DeviceInformation.FindAllAsync(GattDeviceService.GetDeviceSelectorFromShortId(serviceId), new string[] { "System.Devices.ContainerId" });

			foreach (var device in devices)
			{
				cars.Add(new BluetoothLECar(device));
			}

            var devicesLong = await DeviceInformation.FindAllAsync(GattDeviceService.GetDeviceSelectorFromUuid(serviceUuid), new string[] { "System.Devices.ContainerId" });

            foreach (var device in devicesLong)
            {
                cars.Add(new BluetoothLECar(device));
            }

			return cars;
		}

		public IAsyncOperation<IEnumerable<ICar>> FindAvailableCars()
		{
			return FindAvailableCarsAsync().AsAsyncOperation();
		}

		private static ushort serviceId = 0xacc0;
        private static Guid serviceUuid = Guid.Parse("a739acc0-f6cd-1692-994a-d66d9e0ce048");
	}
}
