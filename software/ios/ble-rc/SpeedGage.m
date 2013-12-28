#import "SpeedGage.h"

@implementation SpeedGage
{
    UIImage* background;
    UIImage* icon;
    UIImage* pointer;
}

@synthesize speed;

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    
    if (self)
    {
    }
    
    return self;
}

-(float)speedToAngle
{
    float s = self.speed;
    
    if (s < 0.0f)
    {
        s = 0.0f;
    }
    
    if (s > 7.0f)
    {
        s = 7.0f;
    }
    
    s = 110.0f + s * ((320.0f - 110.0f) / 7.0f);
    
    float rad = (float)s / 180.0f * M_PI;
    
    return rad;
}

-(void)drawRect:(CGRect)rect
{
    if (nil == self->background)
    {
        self->background = [UIImage imageNamed:@"speed-background.png"];
        self->pointer = [UIImage imageNamed:@"speed-pointer.png"];
        
        float scale = self->background.size.width / self.bounds.size.width;
        
        self->background = [UIImage imageWithCGImage:self->background.CGImage scale:scale orientation:UIImageOrientationRight];
        self->pointer = [UIImage imageWithCGImage:self->pointer.CGImage scale:scale orientation:UIImageOrientationRight];
    }
    
    float rad = [self speedToAngle];
    
    CGPoint p;
    p.x = 0;
    p.y = 0;
    [self->background drawAtPoint:p];
    
    
    CGPoint centerOrigin;
    centerOrigin.x = pointer.size.width/2;
    centerOrigin.y = pointer.size.height/2;
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextTranslateCTM(context, centerOrigin.x, centerOrigin.y );
    CGContextRotateCTM( context, rad);
    CGContextDrawImage (context, CGRectMake(-centerOrigin.x, -centerOrigin.y, pointer.size.width,pointer.size.height), pointer.CGImage);
}

@end
