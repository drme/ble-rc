using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

namespace SmartRacer.Shared
{
    public delegate void PositionChangedHandler(ThumbStick sender, float position);

    public partial class ThumbStick : UserControl
    {
        public ThumbStick()
        {
            InitializeComponent();

            this.dll.RenderTransform = this.dragTranslation;

            this.Loaded += (x, y) => { Touch.FrameReported += Touch_FrameReported; };
            this.Unloaded += (x, y) => { Touch.FrameReported -= Touch_FrameReported; };
            IsEnabledChanged += (x, y) => { this.idPointer = -1; };
        }

        bool IsControlChild(DependencyObject element)
        {
            DependencyObject parent = element;
			
			while ((parent != this) && (parent != null))
			{
				parent = VisualTreeHelper.GetParent(parent);
			}

			if (parent == this)
			{
				return true;
			}
			else
			{
				return false;
			}
        }

        private void Touch_FrameReported(object sender, TouchFrameEventArgs e)
        {
            if ((Visibility == Visibility.Collapsed) || (false == this.IsEnabled))
            {
                return;
            }

            TouchPointCollection pointCollection = e.GetTouchPoints(this);

            for (int i = 0; i < pointCollection.Count; i++)
            {
                if (idPointer == -1)
                {
                    if ((pointCollection[i].Action == TouchAction.Down) && IsControlChild(pointCollection[i].TouchDevice.DirectlyOver))
                    {
                        idPointer = pointCollection[i].TouchDevice.Id;
                        sp = pointCollection[i].Position;
                    }
                }
                else if ((pointCollection[i].TouchDevice.Id == idPointer) && (pointCollection[i].Action == TouchAction.Up))
                {
                    idPointer = -1;

                    dragTranslation.Y = 0;
                    dragTranslation.X = 0;

                    UpdatePosition(0.0f);
                }
                else if ((pointCollection[i].TouchDevice.Id == idPointer) && (pointCollection[i].Action == TouchAction.Move))
                {
                    Point dp = pointCollection[i].Position;

                    dragTranslation.X = dp.X - sp.X;
                    dragTranslation.Y = dp.Y - sp.Y;

                    int d = 75;

                    if (this.orientation == Orientation.Vertical)
                    {
                        this.dragTranslation.Y = 0;

                        if (dragTranslation.X > d) dragTranslation.X = d;
                        if (dragTranslation.X < -d) dragTranslation.X = -d;

                        UpdatePosition((float)dragTranslation.X / d);
                    }
                    else
                    {
                        this.dragTranslation.X = 0;

                        if (dragTranslation.Y > d) dragTranslation.Y = d;
                        if (dragTranslation.Y < -d) dragTranslation.Y = -d;

                        UpdatePosition((float)dragTranslation.Y / d);
                    }
                }
            }
        }
     
        private void UpdatePosition(float position)
        {
            float e = 0.001f;

            if (Math.Abs(this.position - position) > e)
            {
                this.position = position;

                if (null != this.Moved)
                {
                    this.Moved(this, this.position);
                }
            }
        }

        public float Position
        {
            get
            {
                return this.position;
            }
            set
            {
                this.position = value;
            }
        }

        public Orientation Orientation
        {
            get
            {
                return this.orientation;
            }
            set
            {
                this.orientation = value;
            }
        }

        private TranslateTransform dragTranslation = new TranslateTransform();
        private Point sp;
        private int idPointer = -1;
        private Orientation orientation = Orientation.Horizontal;
        private float position = 0.0f;
        public event PositionChangedHandler Moved;
    }
}
