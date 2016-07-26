package eu.sarunas.apps.android.racecar;

import android.view.MenuItem;

class Light
{
	public Light(MenuItem menuItem, boolean enabled)
	{
		update(menuItem, enabled);
	};

	public boolean swapState()
	{
		this.state = !this.state;

		return this.state;
	};

	public void setState(boolean state)
	{
		this.state = state;
	};

	public void update(MenuItem menuItem, boolean enabled)
	{
		this.menuItem = menuItem;
		this.menuItem.setVisible(enabled);
	};

	private boolean state = false;
	private MenuItem menuItem = null;
};
