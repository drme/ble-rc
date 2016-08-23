package eu.sarunas.apps.android.racecar.utils;

import java.util.ArrayList;
import java.util.List;
import android.annotation.SuppressLint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;
import eu.sarunas.apps.android.racecar.R;
import eu.sarunas.apps.android.racecar.controller.DeviceType;
import eu.sarunas.apps.android.racecar.scanner.DiscoveredDevice;
import eu.sarunas.apps.android.racecar.wifi.WifiCarController;

public class DeviceListAdapter extends BaseAdapter
{
	public DeviceListAdapter(LayoutInflater inflator)
	{
		this.inflator = inflator;
	};

	public synchronized void addFakes()
	{
		addDevice(new DiscoveredDevice("1", "BMW M3 GT2", DeviceType.MiniZ));
		addDevice(new DiscoveredDevice("2", "Porsche 911 GT3", DeviceType.MiniZ));
		addDevice(new DiscoveredDevice("3", "Ferrari 458", DeviceType.MiniZ));

		notifyDataSetChanged();
	};

	public synchronized void removeFakes()
	{
		List<DiscoveredDevice> remove = new ArrayList<DiscoveredDevice>();

		for (DiscoveredDevice car : this.cars)
		{
			if (car.getAddress().length() == 1)
			{
				remove.add(car);
			}
		}

		this.cars.removeAll(remove);

		notifyDataSetChanged();
	};

	public synchronized void addDevice(DiscoveredDevice device)
	{
		for (DiscoveredDevice car : this.cars)
		{
			if (device.getAddress().equals(car.getAddress()))
			{
				return;
			}
		}

		this.cars.add(device);

		notifyDataSetChanged();
	};

	public synchronized DiscoveredDevice getDevice(int position)
	{
		return this.cars.get(position);
	};

	public synchronized void clear()
	{
		this.cars.clear();
	};

	@Override
	public synchronized int getCount()
	{
		return this.cars.size();
	};

	@Override
	public synchronized Object getItem(int i)
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
	public synchronized View getView(int i, View view, ViewGroup viewGroup)
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

		DiscoveredDevice device = this.cars.get(i);

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

		if (device.getAddress().startsWith(WifiCarController.wifiPrefix))
		{
			int length = WifiCarController.wifiPrefix.length();
			
			viewHolder.deviceAddress.setText(device.getAddress().substring(length, 17 + length).toUpperCase());
		}
		else
		{
			viewHolder.deviceAddress.setText(device.getAddress());
		}

		return view;
	};

	private LayoutInflater inflator;
	private List<DiscoveredDevice> cars = new ArrayList<DiscoveredDevice>();
};
