package eu.sarunas.apps.android.racecar.controller;

import eu.sarunas.apps.android.racecar.ble.CharacteristicProxy;

public class LightsController
{
	public LightsController(CharacteristicProxy lightsProxy)
	{
		this.lightsProxy = lightsProxy;
	};

	public void setMainLights(boolean on)
	{
		this.frontLightsOn = on;
		updateLights();
	};

	public void setLeftTurnLights(boolean on)
	{
		this.leftTurnLightOn = on;
		updateLights();
	};

	public void setRightTurnLights(boolean on)
	{
		this.rightTurnLightOn = on;
		updateLights();
	};

	public void setBackLights(boolean on)
	{
		this.backLightsOn = on;
		updateLights();
	};

	public void setReverseLights(boolean on)
	{
		this.reverseLightsOn = on;
		updateLights();
	};

	private void updateLights()
	{
		final int leftTurnLightsId = 3;
		final int rightTurnLightsId = 2;
		final int backLightsId = 1;
		final int frontLightsId = 0;
		final int reverseLightsId = 4;

		this.lights = 0;

		if (true == this.leftTurnLightOn)
		{
			this.lights |= 1 << leftTurnLightsId;
		}

		if (true == this.rightTurnLightOn)
		{
			this.lights |= 1 << rightTurnLightsId;
		}

		if (true == this.frontLightsOn)
		{
			this.lights |= 1 << frontLightsId;
		}

		if (true == this.backLightsOn)
		{
			this.lights |= 1 << backLightsId;
		}

		if (true == this.reverseLightsOn)
		{
			this.lights |= 1 << reverseLightsId;
		}

		this.lightsProxy.writeValue(this.lights);

		if ((null == this.turnsBlinkingThread) && ((true == this.leftTurnLightOn) || (true == this.rightTurnLightOn)))
		{
			this.turnsBlinkingThread = new Thread(new Runnable()
			{
				private void invertLight(int bit)
				{
					if ((LightsController.this.lights & (1 << bit)) > 0)
					{
						LightsController.this.lights &= ~(1 << bit);
					}
					else
					{
						LightsController.this.lights |= (1 << bit);
					}
				};

				@Override
				public void run()
				{
					while (((true == LightsController.this.leftTurnLightOn) || (true == LightsController.this.rightTurnLightOn)))
					{
						if (true == LightsController.this.leftTurnLightOn)
						{
							invertLight(leftTurnLightsId);
						}

						if (true == LightsController.this.rightTurnLightOn)
						{
							invertLight(rightTurnLightsId);
						}

						LightsController.this.lightsProxy.writeValue(LightsController.this.lights);

						try
						{
							Thread.sleep(1000);
						}
						catch (InterruptedException ex)
						{
							ex.printStackTrace();
						}
					}

					LightsController.this.lights &= ~(1 << leftTurnLightsId);
					LightsController.this.lights &= ~(1 << rightTurnLightsId);

					LightsController.this.lightsProxy.writeValue(LightsController.this.lights);

					LightsController.this.turnsBlinkingThread = null;
				};
			});

			this.turnsBlinkingThread.start();
		}
	};

	public void stop()
	{
		this.backLightsOn = false;
		this.frontLightsOn = false;
		this.reverseLightsOn = false;
		this.leftTurnLightOn = false;
		this.rightTurnLightOn = false;
	};

	private CharacteristicProxy lightsProxy = null;
	private boolean backLightsOn = false;
	private boolean frontLightsOn = false;
	private boolean reverseLightsOn = false;
	private boolean leftTurnLightOn = false;
	private boolean rightTurnLightOn = false;
	private Thread turnsBlinkingThread = null;
	private int lights = 0;
};
