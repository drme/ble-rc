package eu.sarunas.apps.android.racecar;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v4.app.NavUtils;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

public class FirmwareFileSelectActivity extends Activity
{
	public final static String EXTRA_FILENAME = "FileActivity.FILENAME";
	public final static String EXTRA_PREFIX = "FileActivity.PREFIX";
	public final static String EXTRA_TYPE = "FileActivity.TYPE";

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		AppSettings.getInstance().updateTitle(this, true);
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_firmware_file_select);

		Intent intent = getIntent();

		String imageType = intent.getStringExtra(FirmwareFileSelectActivity.EXTRA_TYPE);
		String prefix = intent.getStringExtra(FirmwareFileSelectActivity.EXTRA_PREFIX);

		final List<String> filesList = new ArrayList<String>();

		AssetManager manager = getResources().getAssets();

		try
		{
			for (String file : manager.list(""))
			{
				if ((file.startsWith(prefix)) && (file.contains(imageType)))
				{
					filesList.add(file);
				}
			}
		}
		catch (IOException e)
		{
			e.printStackTrace();
		}

		ListView filesListView = (ListView) findViewById(R.id.lw_file);

		filesListView.setOnItemClickListener(new OnItemClickListener()
		{
			@Override
			public void onItemClick(AdapterView<?> parent, View view, int pos, long id)
			{
				Intent intent = new Intent();

				intent.putExtra(FirmwareFileSelectActivity.EXTRA_FILENAME, filesList.get(pos));
				setResult(Activity.RESULT_OK, intent);
				finish();
				overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
			};
		});

		FileAdapter filesAdapter = new FileAdapter(this, filesList);
		filesListView.setAdapter(filesAdapter);
		filesAdapter.notifyDataSetChanged();
	};

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId())
		{
			case android.R.id.home:
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
		super.onBackPressed();
		overridePendingTransition(R.anim.activity_back_in, R.anim.activity_back_out);
	};

	class FileAdapter extends BaseAdapter
	{
		private List<String> files;
		private LayoutInflater inflater;

		public FileAdapter(Context context, List<String> files)
		{
			this.inflater = LayoutInflater.from(context);
			this.files = files;
		};

		@Override
		public int getCount()
		{
			return this.files.size();
		};

		@Override
		public Object getItem(int pos)
		{
			return this.files.get(pos);
		};

		@Override
		public long getItemId(int pos)
		{
			return pos;
		};

		@SuppressLint("InflateParams")
		@Override
		public View getView(int pos, View view, ViewGroup parent)
		{
			ViewGroup group;

			if (view != null)
			{
				group = (ViewGroup) view;
			}
			else
			{
				group = (ViewGroup) this.inflater.inflate(R.layout.element_firmware_file, null);
			}

			String file = this.files.get(pos);

			TextView nameLabel = (TextView) group.findViewById(R.id.name);
			nameLabel.setText(file.replaceAll("[_][0-9]{1,2}[.][0-9]{1,2}[.][0-9]{1,2}[.][0-9]{1,3}[.]bin$", ""));

			return group;
		};
	};
};
