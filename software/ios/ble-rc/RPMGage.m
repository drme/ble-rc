#import "RPMGage.h"

@implementation RPMGage
{
    UIImage* background;
    UIImage* icon;
    UIImage* pointer;
}

@synthesize rpm;

-(id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];

    if (self)
    {
    }

    return self;
}

-(float)rpmToAngle
{
    float r = self.rpm;
    
    if (r < 0.0f)
    {
        r = 0.0f;
    }
    
    if (r > 7.0f)
    {
        r = 8.0f;
    }
    
    r = 122.0f + r * 24.2f;
    
    float rad = r / 180.0f * M_PI;

    return rad;
}

-(void)drawRect:(CGRect)rect
{
    if (nil == self->background)
    {
        self->background = [UIImage imageNamed:@"rpm-background.png"];
        self->pointer = [UIImage imageNamed:@"rpm-pointer.png"];
        
        float scale = self->background.size.width / self.bounds.size.width;
        
        self->background = [UIImage imageWithCGImage:self->background.CGImage scale:scale orientation:UIImageOrientationRight];
        self->pointer = [UIImage imageWithCGImage:self->pointer.CGImage scale:scale orientation:UIImageOrientationRight];
    }
    
    float rad = [self rpmToAngle];
    
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
