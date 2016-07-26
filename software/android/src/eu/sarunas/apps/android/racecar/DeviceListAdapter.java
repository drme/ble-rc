package eu.sarunas.apps.android.racecar;

import java.util.ArrayList;
import java.util.List;
import android.annotation.SuppressLint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

class DeviceListAdapter extends BaseAdapter
{
	public DeviceListAdapter(LayoutInflater inflator)
	{
		this.inflator = inflator;
	};

	public void addFakes()
	{
		addDevice(null, "BMW M3 GT2", DeviceType.MiniZ, null);
		addDevice(null, "Porsche 911 GT3", DeviceType.MiniZ, null);
		addDevice(null, "Ferrari 458", DeviceType.MiniZ, null);

		notifyDataSetChanged();
	};

	public void removeFakes()
	{
		List<ICarController> remove = new ArrayList<ICarController>();

		for (ICarController car : this.cars)
		{
			if (null == car.getHandle())
			{
				remove.add(car);
			}
		}

		this.cars.removeAll(remove);

		notifyDataSetChanged();
	};

	public void addDevice(Object device, String name, DeviceType type, ICarController newCar)
	{
		if (null == device)
		{
			this.cars.add(new DemoCarController(name));
		}
		else
		{
			for (ICarController car : this.cars)
			{
				if (device == car.getHandle())
				{
					return;
				}
			}

			this.cars.add(newCar);
		}

		notifyDataSetChanged();
	};

	public ICarController getDevice(int position)
	{
		return this.cars.get(position);
	};

	public void clear()
	{
		this.cars.clear();
	};

	@Override
	public int getCount()
	{
		return this.cars.size();
	};

	@Override
	public Object getItem(int i)
	{
		return this.cars.get(i);
	};

	@Override
	public long getItemId(int i)
	{
		return i;
	};

	@SuppressLint("InflateParams")
	@Override
	public View getView(int i, View view, ViewGroup viewGroup)
	{
		ViewHolder viewHolder;

		if (null == view)
		{
			view = this.inflator.inflate(R.layout.listitem_device, null);
			viewHolder = new ViewHolder();
			viewHolder.deviceAddress = (TextView) view.findViewById(R.id.device_address);
			viewHolder.deviceName = (TextView) view.findViewById(R.id.device_name);
			view.setTag(viewHolder);
		}
		else
		{
			viewHolder = (ViewHolder) view.getTag();
		}

		ICarController device = this.cars.get(i);

		String deviceName = device.getName();

		if ((deviceName == null) || (deviceName.length() <= 0))
		{
			deviceName = device.getName();
		}

		if ((deviceName != null) && (deviceName.length() > 0))
		{
			viewHolder.deviceName.setText(deviceName);
		}
		else
		{
			viewHolder.deviceName.setText(R.string.unknown_device);
		}

		viewHolder.deviceAddress.setText(device.getAddress());

		return view;
	};

	private LayoutInflater inflator;
	private List<ICarController> cars = new ArrayList<ICarController>();
};
