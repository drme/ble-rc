package eu.sarunas.apps.android.racecar.utils;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class ThumbStickView extends View implements View.OnTouchListener
{
	@SuppressLint("ClickableViewAccessibility")
	public ThumbStickView(Context context, AttributeSet attrs)
	{
		super(context, attrs);

		setOnTouchListener(this);

		this.paint.setAlpha(127);
		this.paint.setAntiAlias(true);
		this.paint.setFilterBitmap(true);
		this.paint.setDither(true);
	};

	@Override
	public void draw(Canvas canvas)
	{
		super.draw(canvas);

		if (null == this.arrowImage)
		{
			return;
		}

		if (true == isEnabled())
		{
			this.paint.setAlpha(127);
		}
		else
		{
			this.paint.setAlpha(27);
		}

		if (false == this.vertical)
		{
			float scale = (float) getHeight() / (float) this.arrowImage.getHeight();
			int newWidth = (int) (scale * this.arrowImage.getWidth() * 0.7f);
			int newHeight = (int) (getHeight() * 0.7f);

			this.size.left = (getWidth() - newWidth) / 2;
			this.size.top = (getHeight() - newHeight) / 2;
			this.size.bottom = this.size.top + newHeight;
			this.size.right = this.size.left + newWidth;

			int dy = (int) (getHeight() * this.posY * 0.1f);

			this.size.top -= dy;
			this.size.bottom -= dy;
		}
		else
		{
			float aspect = (float) this.arrowImage.getHeight() / (float) this.arrowImage.getWidth();
			float scale = (float) getWidth() / (float) this.arrowImage.getWidth();
			int newWidth = (int) (0.9f * scale * this.arrowImage.getWidth() * 0.7f);
			int newHeight = (int) (0.9f * aspect * newWidth);

			this.size.left = (getWidth() - newWidth) / 2;
			this.size.top = (getHeight() - newHeight) / 2;
			this.size.bottom = this.size.top + newHeight;
			this.size.right = this.size.left + newWidth;

			int dx = (int) (getWidth() * this.posX * 0.1f);

			this.size.left += dx;
			this.size.right += dx;
		}

		canvas.drawBitmap(this.arrowImage, null, this.size, this.paint);
	};

	@SuppressLint("ClickableViewAccessibility")
	@Override
	public boolean onTouch(View view, MotionEvent event)
	{
		if (false == isEnabled())
		{
			return true;
		}

		int px = (int) event.getX();
		int py = (int) event.getY();

		switch (event.getAction() & MotionEvent.ACTION_MASK)
		{
			case MotionEvent.ACTION_DOWN:
				this.gestureStartX = px;
				this.gestureStartY = py;
				this.gestureMaxX = (int) (px * 0.8f);
				this.gestureMaxY = (int) (py * 0.8f);
			case MotionEvent.ACTION_MOVE:
				notifyPositionChange(clamp(((float) (px - this.gestureStartX) / (float) this.gestureMaxX)), clamp(((float) (this.gestureStartY - py) / (float) this.gestureMaxY)));
				break;
			case MotionEvent.ACTION_UP:
				notifyPositionChange(0.0f, 0.0f);
				break;
			default:
				break;
		}

		invalidate();

		return true;
	};

	public void setValueChangedHandler(IValueChanged handler)
	{
		this.valueChangeHandler = handler;
	};

	public interface IValueChanged
	{
		public void onValueChanged(float x, float y);
	};

	private void notifyPositionChange(float x, float y)
	{
		this.posX = x;
		this.posY = y;

		if (null != this.valueChangeHandler)
		{
			this.valueChangeHandler.onValueChanged(x, y);
		}
	};

	private float clamp(float value)
	{
		if (value > 0.5f)
		{
			return 0.5f;
		}
		else if (value < -0.5f)
		{
			return -0.5f;
		}

		return value;
	};

	public void setArrowImage(int id)
	{
		this.arrowImage = BitmapFactory.decodeStream(getContext().getResources().openRawResource(id));
	};

	public void setVertical(boolean vertical)
	{
		this.vertical = vertical;
	};

	public void setPosition(float x, float y)
	{
		this.posX = x;
		this.posY = y;
	};

	private Rect size = new Rect();
	private Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
	private float posX = 0.0f;
	private float posY = 0.0f;
	private IValueChanged valueChangeHandler = null;
	private Bitmap arrowImage = null;
	private int gestureStartX = -1;
	private int gestureStartY = -1;
	private int gestureMaxX = -1;
	private int gestureMaxY = -1;
	private boolean vertical = true;
};
