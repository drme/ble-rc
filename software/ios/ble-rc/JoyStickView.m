#import "JoyStickView.h"
#import "SmartRacer-Swift.h"

@implementation JoyStickView
{
	UIImage* pointer;
	float pos;
	id delegate;
	CGPoint touchStart;
};

@synthesize vertical;
@synthesize enabled;

-(void)initStick
{
	self->pos = 0.0f;
	self->vertical = false;
};

-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];

	if (self)
	{
		[self initStick];
	}

	return self;
};

-(id)initWithCoder:(NSCoder*)coder
{
	self = [super initWithCoder:coder];

	if (self)
	{
		[self initStick];
	}

	return self;
};

-(void)setDelegate:(id)subscriberDelegate
{
	self->delegate = subscriberDelegate;
};




-(void)drawRect:(CGRect)rect
{
	if (nil == self->pointer)
	{
		float scale = 1.0f;

		if (false == self.vertical)
		{
			if (AppSettings.applyHue)
			{
				self->pointer = [UtilsEx imageWithImage:@"ThrottleArrow" fixedHue:0.5 alpha:1.0];
//				[self imageWithImage:@"ThrottleArrow" fixedHue:0.5 alpha:1.0];
			}
			else
			{
				self->pointer = [UIImage imageNamed:@"ThrottleArrow"];
			}

			scale = self->pointer.size.height / self.bounds.size.width * 2.0f;
		}
		else
		{
			if (AppSettings.applyHue)
			{
				self->pointer = [UtilsEx imageWithImage:@"SteeringArrow" fixedHue:0.5 alpha:1.0];
			}
			else
			{
				self->pointer = [UIImage imageNamed:@"SteeringArrow"];
			}

			scale = self->pointer.size.width / self.bounds.size.height * 1.55f;
		}

		self->pointer = [UIImage imageWithCGImage:self->pointer.CGImage scale:scale orientation:UIImageOrientationRight];
	}

	CGPoint p;
	p.x = (self.bounds.size.width - self->pointer.size.width) / 2;
	p.y = (self.bounds.size.height - self->pointer.size.height) / 2;

	if (true == self.enabled)
	{
		if (false == self.vertical)
		{
			p.x += (self->pos * (self.bounds.size.width / 20));
		}
		else
		{
			p.y += (self->pos * (self.bounds.size.height / 10));
		}
	}

	[self->pointer drawAtPoint:p];
};

-(void)notifyDir:(CGPoint)dir
{
	if (nil != self->delegate)
	{
		if (false == self.vertical)
		{
			[self->delegate onStickChanged:self:dir.x];
		}
		else
		{
			[self->delegate onStickChanged:self:dir.y];
		}
	}
};

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (1 != [touches count])
	{
		return;
	}

	UITouch* touch = [touches anyObject];
	UIView * view = [touch view];

	if (self != view)
	{
		return;
	}

	self->touchStart = [touch locationInView:view];
};

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (1 != [touches count])
	{
		return;
	}

	UITouch* touch = [touches anyObject];
	UIView * view = [touch view];

	if (self != view)
	{
		return;
	}

	CGPoint touchPoint = [touch locationInView:view];

	float dx = touchPoint.x - self->touchStart.x;
	float dy = touchPoint.y - self->touchStart.y;

	CGPoint dir;
	dir.x = (dx);
	dir.y = dy;

	if (dx < 0)
	{
		dir.x /= self->touchStart.x * 0.9;
	}
	else
	{
		dir.x /= (view.bounds.size.width - self->touchStart.x) * 0.9;
	}

	if (dy < 0)
	{
		dir.y /= self->touchStart.y * 0.9;
	}
	else
	{
		dir.y /= (view.bounds.size.height - self->touchStart.y) * 0.9;
	}

	if (dir.x < -1)
	{
		dir.x = -1;
	}

	if (dir.x > 1)
	{
		dir.x = 1;
	}

	if (dir.y < -1)
	{
		dir.y = -1;
	}

	if (dir.y > 1)
	{
		dir.y = 1;
	}

	if (false == self.vertical)
	{
		if (fabs(self->pos - dir.x) > 0.001)
		{
			self->pos = dir.x;

			[self setNeedsDisplay];
		}
		else
		{
			self->pos = dir.x;
		}
	}
	else
	{
		if (fabs(self->pos - dir.y) > 0.001)
		{
			self->pos = dir.y;

			[self setNeedsDisplay];
		}
		else
		{
			self->pos = dir.y;
		}
	}

	[self notifyDir:dir];
};

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	CGPoint  dir;
	dir.x = 0.0f;
	dir.y = 0.0f;

	self->pos = 0.0f;

	[self notifyDir:dir];

	[self setNeedsDisplay];
};

@end
