#import "JoyStickView.h"

@implementation JoyStickView
{
    UIImage* background;
    UIImage* pointer;
    float pos;
}

-(void)initStick
{
    self->pos = 0.0f;
}

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if (self)
    {
        [self initStick];
    }

    return self;
}

-(id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
    
    if (self)
	{
        [self initStick];
    }
	
    return self;
}

-(void)drawRect:(CGRect)rect
{
    if (nil == self->background)
    {
        self->background = [UIImage imageNamed:@"slider_back.png"];
        self->pointer = [UIImage imageNamed:@"slider_thumb.png"];
        
        
        float scale = self->background.size.height / self.bounds.size.width * 1.07f;
        
        self->background = [UIImage imageWithCGImage:self->background.CGImage scale:scale orientation:UIImageOrientationRight];
        self->pointer = [UIImage imageWithCGImage:self->pointer.CGImage scale:scale orientation:UIImageOrientationRight];
      
    }
    
    CGPoint p;
    p.x = (self.bounds.size.width - self->background.size.width) / 2;
    p.y = (self.bounds.size.height - self->background.size.height) / 2;
    [self->background drawAtPoint:p];
    
    p.x += (self.bounds.size.width / 2) + (self->pos * (self.bounds.size.width / 2)) - (self.bounds.size.width / 3) - 10;
    
    [self->pointer drawAtPoint:p];
}

-(void)notifyDir:(CGPoint)dir
{
    NSValue* vdir = [NSValue valueWithCGPoint:dir];
    NSDictionary* userInfo = [NSDictionary dictionaryWithObjectsAndKeys:vdir, @"dir", nil];
    
    NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];
    [notificationCenter postNotificationName:@"StickChanged" object:nil userInfo:userInfo];
    
    [self setNeedsDisplay];
}

-(void)touchEvent:(NSSet*)touches
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
    
    float center = self.bounds.size.width / 2;
    CGPoint touchPoint = [touch locationInView:view];
    CGPoint dir;
    dir.x = (touchPoint.x - center) * 0.9;
    double len = sqrt(dir.x * dir.x + dir.y * dir.y);

    double len_inv = (1.0 / len);
    dir.x /= center;
    dir.y *= len_inv;
    
  /*  if (dir.x > 1.0)
    {
        dir.x = 1.0;
    }
    
    if (dir.x < -1)
    {
        dir.x = -1;
    }*/
    
    NSLog(@"X: %f", dir.x, nil);
    
    if (fabs(self->pos - dir.x) > 0.001)
    {
        self->pos = dir.x;
        
        [self setNeedsDisplay];
    }
    else
    {
        self->pos = dir.x;
    }
    
    dir.x *= 1.5f;
    
    if (dir.x < -1)
    {
        dir.x = -1;
    }
    
    if (dir.x > 1)
    {
        dir.x = 1;
    }
    
    [self notifyDir:dir];
}

-(void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    [self touchEvent:touches];
}

-(void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
    [self touchEvent:touches];
}

-(void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
    CGPoint  dir;
    dir.x = 0;
    
    [self notifyDir:dir];
    
    self->pos = 0.0f;
    
    [self setNeedsDisplay];
    
    NSLog(@"Ended");
}

@end
