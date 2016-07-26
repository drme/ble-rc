package eu.sarunas.apps.android.racecar.ble;

import java.util.ArrayList;
import java.util.List;
import android.content.Context;
import eu.sarunas.apps.android.racecar.Scanner;
import eu.sarunas.apps.android.racecar.wifi.WiFiScanner;

public class CompositeScanner extends Scanner
{
	public CompositeScanner(Scanner scanner, WiFiScanner wiFiScanner)
	{
		this.scanners.add(scanner);
		this.scanners.add(wiFiScanner);
	};

	@Override
	public void start(IScanCallback callBack, long timeOut, Context context)
	{
		for (Scanner scanner : this.scanners)
		{
			scanner.start(callBack, timeOut, context);
		}
	};

	@Override
	public void stop()
	{
		for (Scanner scanner : this.scanners)
		{
			scanner.stop();
		}
	};

	private List<Scanner> scanners = new ArrayList<Scanner>();
};
