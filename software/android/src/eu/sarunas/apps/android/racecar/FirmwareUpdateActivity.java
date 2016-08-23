package eu.sarunas.apps.android.racecar;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.NavUtils;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import eu.sarunas.apps.android.racecar.controller.CarsController;
import eu.sarunas.apps.android.racecar.controller.ICarController;
import eu.sarunas.apps.android.racecar.firmware.IFirmwareUpdater;
import eu.sarunas.apps.android.racecar.firmware.IUpdateProgressListener;
import eu.sarunas.apps.android.racecar.firmware.ImageHeader;
import eu.sarunas.apps.android.racecar.firmware.UpdateState;
import eu.sarunas.apps.android.racecar.utils.AppSettings;

@TargetApi(18)
public class FirmwareUpdateActivity extends Activity
{
	@SuppressWarnings("deprecation")
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		AppSettings.getInstance().updateTitle(this, true);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_firmware_update);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		this.progressBar = (ProgressBar) findViewById(R.id.update_progress);

		if (Target.current == AppType.SmartRacer)
		{
			this.progressBar.setProgressDrawable(getResources().getDrawable(R.drawable.progress_horizontal_smart_racer));
		}
		else
		{
			this.progressBar.setProgressDrawable(getResources().getDrawable(R.drawable.progress_horizontal_ble_zee));
		}

		ICarController car = CarsController.getInstance().getConnectedCar();

		this.flashFirmwareButton = (Button) findViewById(R.id.start_update_button);
		this.flashFirmwareButton.setEnabled(false);
		this.selectFirmwareButton = (Button) findViewById(R.id.select_firmware_button);
		this.selectFirmwareButton.setEnabled(null != car);

		this.firmwareInfo = (TextView) findViewById(R.id.firmware_info);
		this.hardwareInfo = (TextView) findViewById(R.id.hardware_info);
		this.newFirmwareInfo = (TextView) findViewById(R.id.new_firmware_info);
		this.progressInfo = (TextView) findViewById(R.id.update_info_log);

		updateGui();
	};

	@Override
	protected void onStop()
	{
		cancelUpdate();
		super.onStop();
	};

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
			case android.R.id.home:
				cancelUpdate();
				NavUtils.navigateUpFromSameTask(this);
				overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
				break;
			default:
				return super.onOptionsItemSelected(item);
		}

		return true;
	};

	@Override
	public void onBackPressed()
	{
		cancelUpdate();

		super.onBackPressed();

		overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
	};

	private void cancelUpdate()
	{
		ICarController car = CarsController.getInstance().getConnectedCar();

		if (null != car)
		{
			car.getUpdater().cancelUpdate();
		}

		updateGui();
	};

	public void startUpdate(View view)
	{
		ICarController car = CarsController.getInstance().getConnectedCar();

		if (null != car)
		{
			if (car.getUpdater().state != UpdateState.Running)
			{
				startProgramming();
			}
			else
			{
				stopProgramming();
			}
		}

		updateGui();
	};

	public void selectFirmware(View view)
	{
		ICarController car = CarsController.getInstance().getConnectedCar();

		if (null != car)
		{
			Intent intent = new Intent(this, Target.firmwareSelectClass);

			String typeFilter = "-";

			if (car.getUpdater().getCurrentImageVersion() == 1)
			{
				typeFilter = "-a-";
			}
			else if (car.getUpdater().getCurrentImageVersion() == 0)
			{
				typeFilter = "-b-";
			}

			intent.putExtra(FirmwareFileSelectActivity.EXTRA_PREFIX, car.getType().getFirmwarePrefix());
			intent.putExtra(FirmwareFileSelectActivity.EXTRA_TYPE, typeFilter);

			startActivityForResult(intent, FirmwareUpdateActivity.FILE_ACTIVITY_REQ);
			overridePendingTransition(R.anim.activity_in, R.anim.activity_out);
		}

		updateGui();
	};

	private void startProgramming()
	{
		updateGui();

		ICarController car = CarsController.getInstance().getConnectedCar();

		if (null != car)
		{
			IFirmwareUpdater updater = car.getUpdater();

			if (null != updater)
			{
				updater.startUpdate(this.fileBuffer, new IUpdateProgressListener()
				{
					@Override
					public void onProgressed(final float percent, final float timeLeft)
					{
						runOnUiThread(new Runnable()
						{
							@Override
							public void run()
							{
								FirmwareUpdateActivity.this.progressBar.setProgress((int) (percent * 100.0f));
								FirmwareUpdateActivity.this.progressInfo.setText(String.format("Time left: %d:%02d", (int) (timeLeft / 60.0), (int) timeLeft % 60));
							}
						});
					};

					@Override
					public void onFinished()
					{
						setResult(Activity.RESULT_OK, new Intent());						
						finish();
						overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
					};

					@Override
					public void onFailed()
					{
						showMessage("Error", "Firmware update failed");
					};

					@Override
					public void onBadImage()
					{
						showMessage("Error", "Wrong image type was selected");
					};
				});
			}
		}
		;
	};

	private void stopProgramming()
	{
		cancelUpdate();
		updateGui();
	};

	private void updateGui()
	{
		ICarController car = CarsController.getInstance().getConnectedCar();

		if (null != car)
		{
			this.hardwareInfo.setText(car.getUpdater().getHardwareVersion());
			this.firmwareInfo.setText(car.getUpdater().getFirmwareVersion());

			if (car.getUpdater().state == UpdateState.Running)
			{
				this.selectFirmwareButton.setEnabled(false);
				this.flashFirmwareButton.setEnabled(true);
				this.flashFirmwareButton.setText(R.string.cancel);
			}
			else
			{
				this.progressBar.setProgress(0);
				this.progressInfo.setText("");
				this.selectFirmwareButton.setEnabled(car.getUpdater().getImage().isAvailable());
				this.flashFirmwareButton.setEnabled(this.fileBuffer != null);
				this.flashFirmwareButton.setText(R.string.update);
			}

			if (null != this.fileBuffer)
			{
				int imgVer = (this.fileHeader.ver) >> 1;
				int imgSize = this.fileHeader.len * 4;

				this.newFirmwareInfo.setText(String.format("Type: %c Ver.: %d Size: %d", this.fileHeader.imgType, imgVer, imgSize));
			}
			else
			{
				this.newFirmwareInfo.setText("");
			}
		}
		else
		{
			this.hardwareInfo.setText("");
			this.firmwareInfo.setText("");
			this.newFirmwareInfo.setText("");
			this.progressBar.setProgress(0);
			this.progressInfo.setText("");
			this.selectFirmwareButton.setEnabled(false);
			this.flashFirmwareButton.setEnabled(false);
		}
	};

	private byte[] getBytes(InputStream stream) throws IOException
	{
		ByteArrayOutputStream buffer = new ByteArrayOutputStream();

		int nRead;
		byte[] data = new byte[16384];

		while ((nRead = stream.read(data, 0, data.length)) != -1)
		{
			buffer.write(data, 0, nRead);
		}

		buffer.flush();

		return buffer.toByteArray();
	};

	private void loadFile(String filepath)
	{
		try
		{
			ICarController car = CarsController.getInstance().getConnectedCar();

			if (null != car)
			{
				InputStream stream = getAssets().open(filepath);

				this.fileBuffer = getBytes(stream);

				stream.close();

				this.fileHeader = car.getUpdater().getHeader(this.fileBuffer);

				if (car.getUpdater().getCurrentImageVersion() == this.fileHeader.ver)
				{
					this.fileHeader = null;
					this.fileBuffer = null;

					showMessage("Error", "Wrong image type was selected");
				}
			}
			else
			{
				this.fileHeader = null;
				this.fileBuffer = null;
			}
		}
		catch (IOException e)
		{
			showMessage("Error", "Error while reading firmware file");

			this.fileHeader = null;
			this.fileBuffer = null;
		}

		updateGui();
	};

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		if (requestCode == FirmwareUpdateActivity.FILE_ACTIVITY_REQ)
		{
			if (resultCode == Activity.RESULT_OK)
			{
				String filename = data.getStringExtra(FirmwareFileSelectActivity.EXTRA_FILENAME);

				loadFile(filename);
			}
		}
	};

	private void showMessage(final String title, final String message)
	{
		runOnUiThread(new Runnable()
		{
			@Override
			public void run()
			{
				AlertDialog alertDialog = new AlertDialog.Builder(FirmwareUpdateActivity.this).create();
				alertDialog.setTitle(title);
				alertDialog.setMessage(message);

				alertDialog.setButton(DialogInterface.BUTTON_POSITIVE, "Ok", new DialogInterface.OnClickListener()
				{
					@Override
					public void onClick(DialogInterface dialog, int whichButton)
					{
					}
				});

				alertDialog.show();
			};
		});
	};

	private ImageHeader fileHeader = null;
	private byte[] fileBuffer = null;
	private ProgressBar progressBar = null;
	private Button selectFirmwareButton = null;
	private Button flashFirmwareButton = null;
	private TextView progressInfo;
	private TextView hardwareInfo;
	private TextView firmwareInfo;
	private TextView newFirmwareInfo;
	private static final int FILE_ACTIVITY_REQ = 0;
};
