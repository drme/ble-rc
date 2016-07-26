using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Devices.Enumeration.Pnp;
using Windows.Storage.Streams;
using Windows.UI.Xaml;

namespace SmartRacerShared
{
    public delegate void DeviceConnectionUpdatedHandler(bool isConnected);

	public sealed class MiniZService
	{
		public MiniZService()
		{
		}

		public MiniZService(Application application)
		{
			application.Suspending += this.AppSuspending;
			application.Resuming += this.AppResuming;
		}

		private async void InitializeService(DeviceInformation device)
		{
			try
			{
				this.deviceContainerId = "{" + device.Properties["System.Devices.ContainerId"] + "}";

				this.service = await GattDeviceService.FromIdAsync(device.Id);

				if (this.service != null)
				{
					this.IsServiceInitialized = true;
					this.steeringCharacteristic = this.service.GetCharacteristics(GattDescriptor.ConvertShortIdToUuid(0xacc2))[0];
					this.throttleCharacteristic = this.service.GetCharacteristics(GattDescriptor.ConvertShortIdToUuid(0xacc1))[0];
					this.SetSteering(1020);
					this.SetThrottle(350);
				}
				else
				{
					Debug.WriteLine("Access to the device is denied, because the application was not granted access or the device is currently in use by another application.");
				}
			}
			catch (Exception ex)
			{
				Debug.WriteLine(ex.StackTrace.ToString());

				Debug.WriteLine("ERROR: Accessing your device failed." + Environment.NewLine + ex.Message);
			}
		}

		public async void FindAndConnect()
		{
//			Task.Factory.StartNew(new Action(async () =>
			{
				String sid = GattDeviceService.GetDeviceSelectorFromShortId(serviceId);

				var devices = await DeviceInformation.FindAllAsync(GattDeviceService.GetDeviceSelectorFromShortId(serviceId), new string[] { "System.Devices.ContainerId" });

				if (devices.Count > 0)
				{
					InitializeService(devices[0]);
				}
			}
			//));
		}

		public void SetSteering(ushort value)
		{
            
			if (value != this.steereing)
			{
				this.steereing = value;
			}
			else
			{
				return;
			}
           

			if (null != this.steeringCharacteristic)
			{
		//		Debug.WriteLine("S: " + value);

				DataWriter writer = new DataWriter();
				UInt16 v = (UInt16)(value >> 8);
				v |= (UInt16)(value << 8);
				writer.WriteUInt16(v);

				Task.Factory.StartNew(new Action(async () => { await this.steeringCharacteristic.WriteValueAsync(writer.DetachBuffer(), GattWriteOption.WriteWithoutResponse); }));
			}
		}

		public void SetThrottle(ushort value)
		{
          
			if (value != this.throttle)
			{
				this.throttle = value;
			}
			else
			{
				return;
			}
           
			if (null != this.throttleCharacteristic)
			{
			//	Debug.WriteLine("T: " + value);

				DataWriter writer = new DataWriter();
				UInt16 v = (UInt16)(value >> 8);
				v |= (UInt16)(value << 8);
				writer.WriteUInt16(v);

				Task.Factory.StartNew(new Action(async () => { await this.throttleCharacteristic.WriteValueAsync(writer.DetachBuffer(), GattWriteOption.WriteWithoutResponse); }));
			}
		}

		private void AppResuming(Object sender, Object e)
		{
			// Since the Windows Runtime will close resources to the device when the app is suspended,
			// the device needs to be reinitialized when the app is resumed.
		}

		private void AppSuspending(Object sender, SuspendingEventArgs e)
		{
			this.IsServiceInitialized = false;

			// This is an appropriate place to save to persistent storage any datapoint the application cares about.
			// For the purpose of this sample we just discard any values.
			
			// Allow the GattDeviceService to get cleaned up by the Windows Runtime.
			// The Windows runtime will clean up resources used by the GattDeviceService object when the application is
			// suspended. The GattDeviceService object will be invalid once the app resumes, which is why it must be 
			// marked as invalid, and reinitalized when the application resumes.
			if (this.service != null)
			{
				this.service.Dispose();
				this.service = null;
			}

			this.steeringCharacteristic = null;
			this.throttleCharacteristic = null;


			if (this.watcher != null)
			{
				this.watcher.Stop();
				this.watcher = null;
			}
		}

		/// <summary>
		/// Register to be notified when a connection is established to the Bluetooth device.
		/// </summary>
		private void StartDeviceConnectionWatcher()
		{
			this.watcher = PnpObject.CreateWatcher(PnpObjectType.DeviceContainer, new string[] { "System.Devices.Connected" }, String.Empty);
			this.watcher.Updated += (PnpObjectWatcher sender, PnpObjectUpdate args) =>
			{
				var connectedProperty = args.Properties["System.Devices.Connected"];

				bool isConnected = false;

				if ((deviceContainerId == args.Id) && Boolean.TryParse(connectedProperty.ToString(), out isConnected) && isConnected)
				{
					this.IsServiceInitialized = true;
					// Once the Client Characteristic Configuration Descriptor is set, the watcher is no longer required
					this.watcher.Stop();
					this.watcher = null;

					// Notifying subscribers of connection state updates
					if (this.DeviceConnectionUpdated != null)
					{
						this.DeviceConnectionUpdated(isConnected);
					}
				}
			};

			this.watcher.Start();
		}

		public bool IsServiceInitialized
		{
			get;
			set;
		}

		private static ushort serviceId = 0xacc0;
		private GattCharacteristic steeringCharacteristic = null;
		private GattCharacteristic throttleCharacteristic = null;
		private GattDeviceService service = null;
		private PnpObjectWatcher watcher = null;
		private String deviceContainerId = null;
		private ushort throttle = 350;
		private ushort steereing = 350;
		public event DeviceConnectionUpdatedHandler DeviceConnectionUpdated;
	}
}
