#import "TempGage.h"

@implementation TempGage
{
    UIImage* background;
    UIImage* icon;
    UIImage* pointer;
}

@synthesize temperature;

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if (self)
    {
    }
    
    return self;
}

-(float)temperatureToAngle
{
    float t = self.temperature;
    
    if (t < 60)
    {
        t = 60;
    }
    
    if (t > 120)
    {
        t = 120;
    }
    
  //  60 - 171;
    
   // 90 - 223
    
   // 120 - 286
    
    t = 171 + (t - 60) * 2;
    
    
    float rad = (float)t / 180.0f * M_PI;
    
    return rad;
}

-(void)drawRect:(CGRect)rect
{
    if (nil == self->background)
    {
        self->background = [UIImage imageNamed:@"temp-background.png"];
        self->pointer = [UIImage imageNamed:@"temp-pointer.png"];
        self->icon = [UIImage imageNamed:@"temp-icon.png"];
        
        float scale = self->background.size.width / self.bounds.size.width;
        
        self->background = [UIImage imageWithCGImage:self->background.CGImage scale:scale orientation:UIImageOrientationRight];
        self->pointer = [UIImage imageWithCGImage:self->pointer.CGImage scale:scale orientation:UIImageOrientationRight];
        self->icon = [UIImage imageWithCGImage:self->icon.CGImage scale:scale orientation:UIImageOrientationRight];
    }
    
    float rad = [self temperatureToAngle];
    
    CGPoint p;
    p.x = 0;
    p.y = 0;
    [self->background drawAtPoint:p];
    [self->icon drawAtPoint:p];
    
    CGPoint centerOrigin;
    centerOrigin.x = pointer.size.width/2;
    centerOrigin.y = pointer.size.height/2;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextTranslateCTM(context, centerOrigin.x, centerOrigin.y );
    CGContextRotateCTM( context, rad);
    CGContextDrawImage (context, CGRectMake(-centerOrigin.x, -centerOrigin.y, pointer.size.width,pointer.size.height), pointer.CGImage);
}

@end
