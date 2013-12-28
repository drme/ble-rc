#import "FuelGage.h"

@implementation FuelGage
{
    UIImage* background;
    UIImage* icon;
    UIImage* pointer;
}

@synthesize batteryLevel;

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if (self)
    {
    }
    return self;
}

-(float)batteryLevelToAngle
{
    float p = self.batteryLevel;
    
    if (p < 0.0f)
    {
        p = 0.0f;
    }
    
    if (p > 100.0f)
    {
        p = 100.0f;
    }
    
    p = 168.0f + p * 1.2f;
    
    float rad = (float)p / 180.0f * M_PI;
    
    return rad;
}

-(void)drawRect:(CGRect)rect
{
    if (nil == self->background)
    {
        self->background = [UIImage imageNamed:@"battery-background.png"];
        self->pointer = [UIImage imageNamed:@"battery-pointer.png"];
        self->icon = [UIImage imageNamed:@"battery-icon.png"];
        
        float scale = self->background.size.width / self.bounds.size.width;
        
        self->background = [UIImage imageWithCGImage:self->background.CGImage scale:scale orientation:UIImageOrientationRight];
        self->pointer = [UIImage imageWithCGImage:self->pointer.CGImage scale:scale orientation:UIImageOrientationRight];
        self->icon = [UIImage imageWithCGImage:self->icon.CGImage scale:scale orientation:UIImageOrientationRight];
    }
    
    float rad = [self batteryLevelToAngle];
    
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
