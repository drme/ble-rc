using SmartRacer;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace SmartRacerShared
{
	public class WiFiDirectCar : Settings, ICar
	{
		public float Throttle
		{
			get
			{
				return 0;
			}
			set
			{
				System.Diagnostics.Debug.WriteLine("T: " + value);

				this.lastThrottle = value;
				SendState();
			}
		}

		public float Steering
		{
			get
			{
				return 0;
			}
			set
			{
				System.Diagnostics.Debug.WriteLine("S: " + value);

				this.lastSteering = value;
				SendState();
			}
		}

		public ILights Lights
		{
			get { throw new NotImplementedException(); }
		}

		public Status Status
		{
			get { throw new NotImplementedException(); }
		}

		public new Settings Settings
		{
			get
			{
				return this;
			}
		}

		public void Connect()
		{

			string result = string.Empty;

			this.socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

			SocketAsyncEventArgs socketEventArg = new SocketAsyncEventArgs();
			socketEventArg.RemoteEndPoint = new DnsEndPoint("192.168.1.1", 80);

			socketEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(delegate(object s, SocketAsyncEventArgs e)
			{
				// Retrieve the result of this request
				result = e.SocketError.ToString();

				// Signal that the request is complete, unblocking the UI thread
		//		_clientDone.Set();

				if (e.SocketError == SocketError.Success)
				{
					if (null != this.Connected)
					{
						this.Connected(this, this);
					}
				}

			});



			// Sets the state of the event to nonsignaled, causing threads to block
//			_clientDone.Reset();

			// Make an asynchronous Connect request over the socket
			this.socket.ConnectAsync(socketEventArg);

			// Block the UI thread for a maximum of TIMEOUT_MILLISECONDS milliseconds.
			// If no response comes back within this time then proceed
//			_clientDone.WaitOne(TIMEOUT_MILLISECONDS);

//			return result;			



		}

		public void Disconnect()
		{
		//	throw new NotImplementedException();

			this.socket.Close();

			if (null != this.Disconnected)
			{
				this.Disconnected(this, this);
			}
		}

		public void TryThrottle(ushort value)
		{
			throw new NotImplementedException();
		}

		public void TrySteering(ushort value)
		{
			throw new NotImplementedException();
		}

		public void BeginSetup()
		{
		//	throw new NotImplementedException();
		}

		public void EndSettup()
		{
			//throw new NotImplementedException();
		}

		public string Name
		{
			get
			{
				return this.name;
			}
		}

		public string Address
		{
			get
			{
				return this.address;
			}
		}

		private string Send(byte[] data)
		{
			string response = "Operation Timeout";

			// We are re-using the _socket object initialized in the Connect method
			if (null != this.socket)
			{
				// Create SocketAsyncEventArgs context object
				SocketAsyncEventArgs socketEventArg = new SocketAsyncEventArgs();

				// Set properties on context object
				socketEventArg.RemoteEndPoint = this.socket.RemoteEndPoint;
				socketEventArg.UserToken = null;

				// Inline event handler for the Completed event.
				// Note: This event handler was implemented inline in order 
				// to make this method self-contained.
				socketEventArg.Completed += new EventHandler<SocketAsyncEventArgs>(delegate(object s, SocketAsyncEventArgs e)
				{
					response = e.SocketError.ToString();

					// Unblock the UI thread
				//	_clientDone.Set();
				});

				// Add the data to be sent into the buffer
				byte[] payload = data;//Encoding.UTF8.GetBytes(data);
				socketEventArg.SetBuffer(payload, 0, payload.Length);

				System.Diagnostics.Debug.WriteLine("Data: " + data[0] + " " + data[1]);

				// Sets the state of the event to nonsignaled, causing threads to block
//				_clientDone.Reset();

				// Make an asynchronous Send request over the socket
				this.socket.SendAsync(socketEventArg);

				// Block the UI thread for a maximum of TIMEOUT_MILLISECONDS milliseconds.
				// If no response comes back within this time then proceed
//				_clientDone.WaitOne(TIMEOUT_MILLISECONDS);
			}
			else
			{
				response = "Socket is not initialized";
			}

			return response;
		}

		private void SendState()
		{
			byte[] data = new byte[2];

			data[0] = (byte)(255.0f * (this.lastSteering + 1.0f) / 2.0f);
			data[1] = (byte)(255.0f * (this.lastThrottle + 1.0f) / 2.0f);

			Send(data);
		}

		public event EventHandler<ICar> Connected;
		public event EventHandler<ICar> Disconnected;
		public event EventHandler<ICar> StatusUpdated;
		private Socket socket = null;
		private float lastThrottle = 0.0f;
		private float lastSteering = 0.0f;
		private String name = "WiFi Car";
		private String address = "192.168.1.1";

		public override void Save()
		{
			//throw new NotImplementedException();
		}
	}

	public class WiFiDirectCarFactory : ICarFactory
	{
		public async Task<List<ICar>> FindAvailableCars()
		{
			List<ICar> cars = new List<ICar>();

			cars.Add(new WiFiDirectCar());

			return cars;
		}

		private static ushort serviceId = 0xacc0;
		private static Guid serviceUuid = Guid.Parse("a739acc0-f6cd-1692-994a-d66d9e0ce048");
	}
}
