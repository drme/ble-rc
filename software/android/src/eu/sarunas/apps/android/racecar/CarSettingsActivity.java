package eu.sarunas.apps.android.racecar;

import java.util.Locale;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.app.NavUtils;
import android.support.v4.view.ViewPager;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import eu.sarunas.apps.android.racecar.controller.CarsController;
import eu.sarunas.apps.android.racecar.controller.DeviceType;
import eu.sarunas.apps.android.racecar.utils.AppSettings;
import android.widget.TextView;

public class CarSettingsActivity extends FragmentActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		AppSettings.getInstance().updateTitle(this, true);
		setContentView(R.layout.activity_car_settings);

		ViewPager viewPager = (ViewPager) findViewById(R.id.pager);
		viewPager.setAdapter(new SectionsPagerAdapter(getSupportFragmentManager()));
	};

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		final ViewPager viewPager = (ViewPager) findViewById(R.id.pager);

		switch (item.getItemId())
		{
			case android.R.id.home:
				NavUtils.navigateUpFromSameTask(this);
				overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
				break;
			case R.id.menu_general_settings:
				viewPager.setCurrentItem(0);
				break;
			case R.id.menu_steering_settings:
				viewPager.setCurrentItem(1);
				break;
			case R.id.menu_throttle_settings:
				viewPager.setCurrentItem(2);
				break;
			default:
				return super.onOptionsItemSelected(item);
		}

		return true;
	};

	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.settings, menu);

		return true;
	};

	public class SectionsPagerAdapter extends FragmentPagerAdapter
	{
		public SectionsPagerAdapter(FragmentManager fragmentManager)
		{
			super(fragmentManager);
		};

		@Override
		public Fragment getItem(int position)
		{
			switch (position)
			{
				case 0:
					return new GeneralSectionFragment();
				case 1:
					return new SteeringSectionFragment();
				case 2:
					return new ThrottleSectionFragment();
				case 3:
					return new BatterySectionFragment();
				default:
					return null;
			}
		};

		@Override
		public int getCount()
		{
			return 3;
		};

		@Override
		public CharSequence getPageTitle(int position)
		{
			Locale locale = Locale.getDefault();

			switch (position)
			{
				case 0:
					return getString(R.string.general).toUpperCase(locale);
				case 1:
					return getString(R.string.steering).toUpperCase(locale);
				case 2:
					return getString(R.string.throttle).toUpperCase(locale);
				case 3:
					return getString(R.string.battery).toUpperCase(locale);
			}
			return null;
		};
	};

	private static void updateLabel(View rootView, Fragment fragment, int id, int textId, int value)
	{
		TextView label = (TextView) rootView.findViewById(id);

		label.setText(fragment.getString(textId) + ": " + value + "");
	};

	public static class SteeringSectionFragment extends Fragment
	{
		private static int fixSteeringValue(int value)
		{
			if (value <= 0)
			{
				return 0;
			}

			if (value > CarsController.getInstance().getConnectedCar().getMaxPosibleSteering())
			{
				return CarsController.getInstance().getConnectedCar().getMaxPosibleSteering();
			}

			return value;
		};

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
		{
			if (null != CarsController.getInstance().getConnectedCar())
			{
				final View rootView = inflater.inflate(R.layout.steering_settings_fragment, container, false);
				final CheckBox symmetry = (CheckBox) rootView.findViewById(R.id.checkBoxSymmetry);

				final SeekBar seekBarCenter = (SeekBar) rootView.findViewById(R.id.seekBarSteeringCenter);
				seekBarCenter.setMax(CarsController.getInstance().getConnectedCar().getMaxPosibleSteering());
				seekBarCenter.setProgress(CarsController.getInstance().getConnectedCar().getCenterSteering());

				final SeekBar seekBarMin = (SeekBar) rootView.findViewById(R.id.seekBarSteeringMin);
				seekBarMin.setMax(CarsController.getInstance().getConnectedCar().getMaxPosibleSteering());
				seekBarMin.setProgress(CarsController.getInstance().getConnectedCar().getMinSteering());

				final SeekBar seekBarMax = (SeekBar) rootView.findViewById(R.id.seekBarSteeringMax);
				seekBarMax.setMax(CarsController.getInstance().getConnectedCar().getMaxPosibleSteering());
				seekBarMax.setProgress(CarsController.getInstance().getConnectedCar().getMaxSteering());

				setupSteeringCenter(rootView, symmetry, seekBarCenter, seekBarMin, seekBarMax);
				setupSterringLeft(rootView, symmetry, seekBarCenter, seekBarMin, seekBarMax);
				setupSteeringRight(rootView, symmetry, seekBarCenter, seekBarMin, seekBarMax);
				setupCenterSteeringOption(rootView, symmetry, seekBarCenter, seekBarMin, seekBarMax);
				setupDescription(rootView, seekBarCenter);

				return rootView;
			}
			else
			{
				return null;
			}
		};

		private void setupDescription(final View rootView, final SeekBar seekBarCenter)
		{
			TextView description = (TextView) rootView.findViewById(R.id.textViewSterringDescription);

			if (DeviceType.Simple == CarsController.getInstance().getConnectedCar().getType())
			{
				description.setText(getString(R.string.steering_hint_simple));

				seekBarCenter.setVisibility(View.INVISIBLE);

				rootView.findViewById(R.id.textViewSteeringCenter).setVisibility(View.INVISIBLE);
			}
			else
			{
				description.setText(getString(R.string.steering_hint));
			}
		};

		private void setupCenterSteeringOption(final View rootView, final CheckBox symmetry, final SeekBar seekBarCenter, final SeekBar seekBarMin, final SeekBar seekBarMax)
		{
			symmetry.setChecked(AppSettings.getInstance().isCenterSteering());
			symmetry.setOnCheckedChangeListener(new OnCheckedChangeListener()
			{
				@Override
				public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
				{
					AppSettings.getInstance().setCenterSteering(isChecked);

					if (true == symmetry.isChecked())
					{
						int diff = (seekBarMax.getProgress() - seekBarMin.getProgress()) / 2;

						int min = SteeringSectionFragment.fixSteeringValue(seekBarCenter.getProgress() - diff);
						int max = SteeringSectionFragment.fixSteeringValue(seekBarCenter.getProgress() + diff);

						CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMin, R.string.steering_left, min);
						CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMax, R.string.steering_right, max);

						CarsController.getInstance().getConnectedCar().setMinSteering(min);
						CarsController.getInstance().getConnectedCar().setMaxSteering(max);

						SteeringSectionFragment.this.disableShoweRight = true;
						SteeringSectionFragment.this.disableShowLeft = true;
						seekBarMin.setProgress(min);
						seekBarMax.setProgress(max);
						SteeringSectionFragment.this.disableShoweRight = false;
						SteeringSectionFragment.this.disableShowLeft = false;
					}
				};
			});
		};

		private void setupSteeringRight(final View rootView, final CheckBox symmetry, final SeekBar seekBarCenter, final SeekBar seekBarMin, final SeekBar seekBarMax)
		{
			seekBarMax.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
			{
				@Override
				public void onStopTrackingTouch(SeekBar seekBar)
				{
					CarsController.getInstance().getConnectedCar().centerSteering();
				};

				@Override
				public void onStartTrackingTouch(SeekBar seekBar)
				{
				};

				@Override
				public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
				{
					CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMax, R.string.steering_right, progress);

					CarsController.getInstance().getConnectedCar().setMaxSteering(progress);

					if (false == SteeringSectionFragment.this.disableShoweRight)
					{
						CarsController.getInstance().getConnectedCar().setSteering(progress);
					}

					if (true == symmetry.isChecked())
					{
						int diff = (progress - seekBarCenter.getProgress());

						int min = SteeringSectionFragment.fixSteeringValue(seekBarCenter.getProgress() - diff);

						CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMin, R.string.steering_left, min);

						CarsController.getInstance().getConnectedCar().setMinSteering(min);

						SteeringSectionFragment.this.disableShowLeft = true;
						seekBarMin.setProgress(min);
						SteeringSectionFragment.this.disableShowLeft = false;
					}
				};
			});
		};

		private void setupSterringLeft(final View rootView, final CheckBox symmetry, final SeekBar seekBarCenter, final SeekBar seekBarMin, final SeekBar seekBarMax)
		{
			seekBarMin.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
			{
				@Override
				public void onStopTrackingTouch(SeekBar seekBar)
				{
					CarsController.getInstance().getConnectedCar().centerSteering();
				};

				@Override
				public void onStartTrackingTouch(SeekBar seekBar)
				{
				};

				@Override
				public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
				{
					CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMin, R.string.steering_left, progress);

					CarsController.getInstance().getConnectedCar().setMinSteering(progress);

					if (false == SteeringSectionFragment.this.disableShowLeft)
					{
						CarsController.getInstance().getConnectedCar().setSteering(progress);
					}

					if (true == symmetry.isChecked())
					{
						int diff = (seekBarCenter.getProgress() - progress);

						int max = SteeringSectionFragment.fixSteeringValue(seekBarCenter.getProgress() + diff);

						CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMax, R.string.steering_right, max);

						CarsController.getInstance().getConnectedCar().setMaxSteering(max);

						SteeringSectionFragment.this.disableShoweRight = true;
						seekBarMax.setProgress(max);
						SteeringSectionFragment.this.disableShoweRight = false;
					}
				};
			});
		};

		private void setupSteeringCenter(final View rootView, final CheckBox symmetry, final SeekBar seekBarCenter, final SeekBar seekBarMin, final SeekBar seekBarMax)
		{
			seekBarCenter.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
			{
				@Override
				public void onStopTrackingTouch(SeekBar seekBar)
				{
					CarsController.getInstance().getConnectedCar().centerSteering();
				};

				@Override
				public void onStartTrackingTouch(SeekBar seekBar)
				{
				};

				@Override
				public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
				{
					CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringCenter, R.string.steering_center, progress);

					CarsController.getInstance().getConnectedCar().setCenterSteering(progress);
					CarsController.getInstance().getConnectedCar().setSteering(progress);

					if (true == symmetry.isChecked())
					{
						int diff = (seekBarMax.getProgress() - seekBarMin.getProgress()) / 2;

						int min = SteeringSectionFragment.fixSteeringValue(progress - diff);
						int max = SteeringSectionFragment.fixSteeringValue(progress + diff);

						CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMin, R.string.steering_left, min);
						CarSettingsActivity.updateLabel(rootView, SteeringSectionFragment.this, R.id.textViewSteeringMax, R.string.steering_right, max);

						CarsController.getInstance().getConnectedCar().setMinSteering(min);
						CarsController.getInstance().getConnectedCar().setMaxSteering(max);

						SteeringSectionFragment.this.disableShoweRight = true;
						SteeringSectionFragment.this.disableShowLeft = true;

						seekBarMin.setProgress(min);
						seekBarMax.setProgress(max);

						SteeringSectionFragment.this.disableShoweRight = false;
						SteeringSectionFragment.this.disableShowLeft = false;
					}
				};
			});
		};

		private boolean disableShowLeft = false;
		private Boolean disableShoweRight = false;
	};

	public static class ThrottleSectionFragment extends Fragment
	{
		public ThrottleSectionFragment()
		{
		};

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
		{
			if (null != CarsController.getInstance().getConnectedCar())
			{
				int maxThrottle = CarsController.getInstance().getConnectedCar().getMaxPosibleThrottle();
				
				final View rootView = inflater.inflate(R.layout.throttle_settings_fragment, container, false);

				TextView description = (TextView) rootView.findViewById(R.id.textViewThrottleDescription);

				SeekBar seekBarCenter = (SeekBar) rootView.findViewById(R.id.seekBarThrottleCenter);
				seekBarCenter.setMax(maxThrottle);
				seekBarCenter.setProgress(CarsController.getInstance().getConnectedCar().getCenterThrottle());
				seekBarCenter.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
				{
					@Override
					public void onStopTrackingTouch(SeekBar seekBar)
					{
						CarsController.getInstance().getConnectedCar().centerThrottle();
					};

					@Override
					public void onStartTrackingTouch(SeekBar seekBar)
					{
					};

					@Override
					public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
					{
						CarSettingsActivity.updateLabel(rootView, ThrottleSectionFragment.this, R.id.textViewThrottleCenter, R.string.throttle_idle, progress);

						CarsController.getInstance().getConnectedCar().setCenterThrottle(progress);
						CarsController.getInstance().getConnectedCar().setThrottle(progress);
					};
				});

				SeekBar seekBarMin = (SeekBar) rootView.findViewById(R.id.seekBarThrottleMin);
				seekBarMin.setMax(maxThrottle);
				seekBarMin.setProgress(CarsController.getInstance().getConnectedCar().getMinThrottle());
				seekBarMin.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
				{
					@Override
					public void onStopTrackingTouch(SeekBar seekBar)
					{
						CarsController.getInstance().getConnectedCar().centerThrottle();
					};

					@Override
					public void onStartTrackingTouch(SeekBar seekBar)
					{
					};

					@Override
					public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
					{
						CarSettingsActivity.updateLabel(rootView, ThrottleSectionFragment.this, R.id.textViewThrottleMin, R.string.throttle_reverse, progress);

						CarsController.getInstance().getConnectedCar().setMinThrottle(progress);
						CarsController.getInstance().getConnectedCar().setThrottle(progress);
					};
				});

				SeekBar seekBarMax = (SeekBar) rootView.findViewById(R.id.seekBarThrottleMax);
				seekBarMax.setMax(maxThrottle);
				seekBarMax.setProgress(CarsController.getInstance().getConnectedCar().getMaxThrottle());
				seekBarMax.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
				{
					@Override
					public void onStopTrackingTouch(SeekBar seekBar)
					{
						CarsController.getInstance().getConnectedCar().centerThrottle();
					};

					@Override
					public void onStartTrackingTouch(SeekBar seekBar)
					{
					};

					@Override
					public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
					{
						CarSettingsActivity.updateLabel(rootView, ThrottleSectionFragment.this, R.id.textViewThrottleMax, R.string.throttle_forward, progress);

						CarsController.getInstance().getConnectedCar().setMaxThrottle(progress);
						CarsController.getInstance().getConnectedCar().setThrottle(progress);
					};
				});

				if ((DeviceType.MiniZ == CarsController.getInstance().getConnectedCar().getType()) || (DeviceType.Simple == CarsController.getInstance().getConnectedCar().getType())  || (DeviceType.dNano == CarsController.getInstance().getConnectedCar().getType()))
				{
					description.setText(getString(R.string.throttle_hint_simple_mini_z));
					seekBarCenter.setVisibility(View.INVISIBLE);
					rootView.findViewById(R.id.textViewThrottleCenter).setVisibility(View.INVISIBLE);
				}
				else
				{
					description.setText(getString(R.string.throttle_hint));
				}

				return rootView;
			}
			else
			{
				return null;
			}
		};
	};

	public static class BatterySectionFragment extends Fragment
	{
		public BatterySectionFragment()
		{
		};

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
		{
			if (null != CarsController.getInstance().getConnectedCar())
			{
				View rootView = inflater.inflate(R.layout.battery_settings_fragment, container, false);

				EditText textEditVoltage = (EditText) rootView.findViewById(R.id.editTextBatteryVoltage);
				textEditVoltage.setText(String.format("%f", CarsController.getInstance().getConnectedCar().getMaxBatteryVoltage() / 1000.0f));
				textEditVoltage.addTextChangedListener(new TextWatcher()
				{
					@Override
					public void onTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
					{
					};

					@Override
					public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
					{
					};

					@Override
					public void afterTextChanged(Editable arg0)
					{
						try
						{
							CarsController.getInstance().getConnectedCar().setMaxBatteryVoltage((int) (Float.parseFloat(arg0.toString()) * 1000));
						}
						catch (Throwable ex)
						{
							ex.printStackTrace();
						}
					};
				});

				EditText textEditCapacity = (EditText) rootView.findViewById(R.id.editTextBatteryCapacity);
				textEditCapacity.setText(String.format("%f", CarsController.getInstance().getConnectedCar().getBatteryCapacity() / 1000.0f));
				textEditCapacity.addTextChangedListener(new TextWatcher()
				{
					@Override
					public void onTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
					{
					};

					@Override
					public void beforeTextChanged(CharSequence arg0, int arg1, int arg2, int arg3)
					{
					};

					@Override
					public void afterTextChanged(Editable arg0)
					{
						try
						{
							CarsController.getInstance().getConnectedCar().setBatteryCapacity((int) (Float.parseFloat(arg0.toString()) * 1000));
						}
						catch (Throwable ex)
						{
							ex.printStackTrace();
						}
					};
				});

				return rootView;
			}
			else
			{
				return null;
			}
		};
	};

	public static class GeneralSectionFragment extends Fragment
	{
		public GeneralSectionFragment()
		{
		};

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
		{
			if (null != CarsController.getInstance().getConnectedCar())
			{
				final View rootView = inflater.inflate(R.layout.general_settings_fragment, container, false);

				SeekBar seekBarPulse = (SeekBar) rootView.findViewById(R.id.seekBarPulseWidth);
				seekBarPulse.setMax(7000);
				seekBarPulse.setProgress(CarsController.getInstance().getConnectedCar().getPulseWidth());
				seekBarPulse.setOnSeekBarChangeListener(new OnSeekBarChangeListener()
				{
					@Override
					public void onStopTrackingTouch(SeekBar seekBar)
					{
					};

					@Override
					public void onStartTrackingTouch(SeekBar seekBar)
					{
					};

					@Override
					public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
					{
						CarSettingsActivity.updateLabel(rootView, GeneralSectionFragment.this, R.id.textViewPulseWidth, R.string.pulse_width, progress);

						CarsController.getInstance().getConnectedCar().setPulseWidth(progress);
					};
				});

				seekBarPulse.setVisibility(View.INVISIBLE);

				((TextView) rootView.findViewById(R.id.textViewPulseWidth)).setVisibility(View.INVISIBLE);

				EditText carName = (EditText) rootView.findViewById(R.id.editTextRadioDeviceName);

				carName.setText(CarsController.getInstance().getConnectedCar().getName());

				carName.addTextChangedListener(new TextWatcher()
				{
					@Override
					public void onTextChanged(CharSequence s, int start, int before, int count)
					{
					};

					@Override
					public void beforeTextChanged(CharSequence s, int start, int count, int after)
					{
					};

					@Override
					public void afterTextChanged(Editable s)
					{
						CarsController.getInstance().getConnectedCar().setName(s.toString());
					};
				});

				CheckBox invertThrottle = (CheckBox) rootView.findViewById(R.id.checkBoxInvertThrottle);
				invertThrottle.setChecked(AppSettings.getInstance().isInvertThrottle());
				invertThrottle.setOnCheckedChangeListener(new OnCheckedChangeListener()
				{
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
					{
						AppSettings.getInstance().setInvertThrottle(isChecked);
					};
				});

				CheckBox invertSteering = (CheckBox) rootView.findViewById(R.id.checkBoxInvertSteering);
				invertSteering.setChecked(AppSettings.getInstance().isInvertSteering());
				invertSteering.setOnCheckedChangeListener(new OnCheckedChangeListener()
				{
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
					{
						AppSettings.getInstance().setInvertSteering(isChecked);
					};
				});

				final RadioGroup group = (RadioGroup) rootView.findViewById(R.id.radioGroupAccelerometerDirection);
				group.setVisibility(AppSettings.getInstance().isUseAccelerometer() ? View.VISIBLE : View.INVISIBLE);

				CheckBox useAccelerometer = (CheckBox) rootView.findViewById(R.id.checkBoxUseAccelerometer);
				useAccelerometer.setChecked(AppSettings.getInstance().isUseAccelerometer());
				useAccelerometer.setOnCheckedChangeListener(new OnCheckedChangeListener()
				{
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
					{
						AppSettings.getInstance().setUseAccelerometer(isChecked);

						group.setVisibility(isChecked ? View.VISIBLE : View.INVISIBLE);
					};
				});

				OnCheckedChangeListener radioButtonHandler = new OnCheckedChangeListener()
				{
					@Override
					public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
					{
						if (true == isChecked)
						{
							AppSettings.getInstance().setAccelerometerAxis((AppSettings.AccelerometerAxis) buttonView.getTag());
						}
					};
				};

				RadioButton useX = (RadioButton) rootView.findViewById(R.id.radioUseX);
				useX.setTag(AppSettings.AccelerometerAxis.X);
				useX.setChecked(useX.getTag().equals(AppSettings.getInstance().getAccelerometerAxis()));
				useX.setOnCheckedChangeListener(radioButtonHandler);

				RadioButton useY = (RadioButton) rootView.findViewById(R.id.radioUseY);
				useY.setTag(AppSettings.AccelerometerAxis.Y);
				useY.setChecked(useY.getTag().equals(AppSettings.getInstance().getAccelerometerAxis()));
				useY.setOnCheckedChangeListener(radioButtonHandler);

				RadioButton useZ = (RadioButton) rootView.findViewById(R.id.radioUseZ);
				useZ.setTag(AppSettings.AccelerometerAxis.Z);
				useZ.setChecked(useZ.getTag().equals(AppSettings.getInstance().getAccelerometerAxis()));
				useZ.setOnCheckedChangeListener(radioButtonHandler);

				return rootView;
			}
			else
			{
				return null;
			}
		};
	};

	@Override
	public void onBackPressed()
	{
		super.onBackPressed();

		overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
	};
};
