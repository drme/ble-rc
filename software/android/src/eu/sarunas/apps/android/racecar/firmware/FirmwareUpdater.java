package eu.sarunas.apps.android.racecar.firmware;

import java.nio.charset.Charset;
import eu.sarunas.apps.android.racecar.ble.CharacteristicProxy;
import eu.sarunas.apps.android.racecar.ble.CommandsQueue;
import eu.sarunas.apps.android.racecar.ble.ICharacteristicDataHandler;

public class FirmwareUpdater extends IFirmwareUpdater
{
	public FirmwareUpdater()
	{
		this.imageNotify = new CharacteristicProxy("F000FFC1-0451-4000-B000-000000000000", FirmwareUpdater.oadServiceId, new ICharacteristicDataHandler()
		{
			@Override
			public void onDataAvailable(CharacteristicProxy proxy, byte[] data)
			{
				System.out.println("got image version");

				if (false == this.gotVersion)
				{
					if (data.length >= 2)
					{
						FirmwareUpdater.this.currentImageVersion = FirmwareUpdater.buildUint16(data[1], data[0]);
						this.gotVersion = true;
					}
				}
			};

			@Override
			public void onConnected(final CharacteristicProxy proxy)
			{
				proxy.setNotify(true);

				CharacteristicProxy.commandsQueue.add(new Runnable()
				{
					@Override
					public void run()
					{
						proxy.writeValue(new byte[] { 0x00 });

						for (int i = 0; (i < 500) && (false == gotVersion); i++)
						{
							CommandsQueue.sleep(10);
						}

						if (false == gotVersion)
						{
							proxy.writeValue(new byte[] { 0x01 });
						}
					};
				});
			}

			private boolean gotVersion = false;
		});

		this.imageBlockId = new CharacteristicProxy("F000FFC2-0451-4000-B000-000000000000", FirmwareUpdater.oadServiceId, new ICharacteristicDataHandler()
		{
			@Override
			public void onDataAvailable(CharacteristicProxy proxy, byte[] data)
			{
				final int number = (((data[1]) << 8) & 0xff00) | ((data[0]) & 0xff);

				System.out.println("Bew lock: " + number);

				if (FirmwareUpdater.this.state == UpdateState.Running)
				{
					runLater(new Runnable()
					{
						@Override
						public void run()
						{
							sendBlock(number);
						}
					}, 1);
				}
			};

			@Override
			public void onConnected(final CharacteristicProxy proxy)
			{
				proxy.setNotify(true);
			};
		});

		this.hardware = new CharacteristicProxy("00002a27-0000-1000-8000-00805f9b34fb", "0000180a-0000-1000-8000-00805f9b34fb", new ICharacteristicDataHandler()
		{
			@Override
			public void onDataAvailable(CharacteristicProxy proxy, byte[] data)
			{
				FirmwareUpdater.this.hardwareVersion = new String(data, Charset.forName("UTF-8"));
			};

			@Override
			public void onConnected(CharacteristicProxy proxy)
			{
				proxy.startRead();
			};
		});

		this.firmware = new CharacteristicProxy("00002a26-0000-1000-8000-00805f9b34fb", "0000180a-0000-1000-8000-00805f9b34fb", new ICharacteristicDataHandler()
		{
			@Override
			public void onDataAvailable(CharacteristicProxy proxy, byte[] data)
			{
				FirmwareUpdater.this.firmwareVersion = new String(data, Charset.forName("UTF-8"));
			};

			@Override
			public void onConnected(CharacteristicProxy proxy)
			{
				proxy.startRead();
			};
		});
	};

	public CharacteristicProxy getBlock()
	{
		return this.imageBlockId;
	};

	public CharacteristicProxy getImage()
	{
		return this.imageNotify;
	};

	public CharacteristicProxy getHardware()
	{
		return this.hardware;
	};

	public CharacteristicProxy getFirmware()
	{
		return this.firmware;
	};

	@Override
	public String getHardwareVersion()
	{
		return this.hardwareVersion;
	};

	@Override
	public String getFirmwareVersion()
	{
		return this.firmwareVersion;
	};

	@Override
	public void cancelUpdate()
	{
		this.state = UpdateState.Canceled;
	};

	private void sendBlock(int blockNumber)
	{
		short iBlocks = (short) blockNumber;
		int iBytes = iBlocks * FirmwareUpdater.oadBlockSize;

		this.oadBuffer[0] = FirmwareUpdater.loUint16(iBlocks);
		this.oadBuffer[1] = FirmwareUpdater.hiUint16(iBlocks);
		System.arraycopy(this.fileBuffer, iBytes, this.oadBuffer, 2, FirmwareUpdater.oadBlockSize);

		this.imageBlockId.writeValue(this.oadBuffer);

		this.progress.onProgressed((float) blockNumber / (float) this.totalBlocks, this.secPerBlock * (this.totalBlocks - blockNumber));

		if (blockNumber >= (this.totalBlocks - 1))
		{
			this.progress.onFinished();
		}
	};

	public ImageHeader getHeader(byte[] fileBuffer)
	{
		ImageHeader header = new ImageHeader();

		header.ver = FirmwareUpdater.buildUint16(fileBuffer[5], fileBuffer[4]);
		header.len = FirmwareUpdater.buildUint16(fileBuffer[7], fileBuffer[6]);
		header.imgType = ((header.ver & 1) == 1) ? 'B' : 'A';

		System.arraycopy(fileBuffer, 8, header.uid, 0, 4);

		return header;
	};

	@Override
	public void startUpdate(byte[] mFileBuffer, IUpdateProgressListener progress)
	{
		this.fileBuffer = mFileBuffer;
		this.progress = progress;
		this.state = UpdateState.Running;

		this.fileHeader = getHeader(mFileBuffer);

		if (this.fileHeader.ver == this.currentImageVersion)
		{
			progress.onBadImage();
		}
		else
		{
			byte[] buf = new byte[FirmwareUpdater.oadImageHeaderSize + 2 + 2];
			buf[0] = FirmwareUpdater.loUint16(this.fileHeader.ver);
			buf[1] = FirmwareUpdater.hiUint16(this.fileHeader.ver);
			buf[2] = FirmwareUpdater.loUint16(this.fileHeader.len);
			buf[3] = FirmwareUpdater.hiUint16(this.fileHeader.len);

			System.arraycopy(this.fileHeader.uid, 0, buf, 4, 4);

			this.totalBlocks = (short) (this.fileHeader.len / (FirmwareUpdater.oadBlockSize / FirmwareUpdater.flashWordSize));
			this.progress.onProgressed(0.0f, this.secPerBlock * this.totalBlocks);

			this.imageNotify.writeValue(buf);
		}
	};

	@Override
	public short getCurrentImageVersion()
	{
		return this.currentImageVersion;
	};

	private void runLater(final Runnable command, final long delay)
	{
		this.queue.add(command);
		this.queue.setBlockCommands(false);
	};

	private static byte loUint16(short v)
	{
		return (byte) (v & 0xFF);
	};

	private static byte hiUint16(short v)
	{
		return (byte) (v >> 8);
	};

	private static short buildUint16(byte hi, byte lo)
	{
		return (short) ((hi << 8) + (lo & 0xff));
	};

	private CommandsQueue queue = new CommandsQueue("Updater thread");
	private float secPerBlock = 0.10f;
	private int totalBlocks = 0;
	private ImageHeader fileHeader = new ImageHeader();
	private byte[] fileBuffer = null;
	private IUpdateProgressListener progress = null;
	private short currentImageVersion = (short) 0xffff;
	private CharacteristicProxy imageBlockId;
	private CharacteristicProxy imageNotify;
	private CharacteristicProxy hardware;
	private CharacteristicProxy firmware;
	private String hardwareVersion = "";
	private String firmwareVersion = "";
	private static final int oadImageHeaderSize = 8;
	private static final int oadBlockSize = 16;
	private static final int flashWordSize = 4;
	private static final int oadBufferSize = 2 + FirmwareUpdater.oadBlockSize;
	private final byte[] oadBuffer = new byte[FirmwareUpdater.oadBufferSize];
	public static final String oadServiceId = "f000ffc0-0451-4000-b000-000000000000";
};
