#import "CarViewController.h"
#import "CarsController.h"

@implementation CarViewController
{
    bool pressed;
    float py;	
	bool lightsOn;
	bool leftOn;
	bool rightOn;
	bool emergencyOn;
}

@synthesize accelerometer;
@synthesize powerButton;
@synthesize leftButton;
@synthesize rightButton;
@synthesize emergencyButton;
@synthesize lightsButton;
@synthesize throttleSlider;
@synthesize tempGage;
@synthesize fuelGage;
@synthesize speedGage;
@synthesize rpmGage;
@synthesize navigationBar;
@synthesize statusBarView;
@synthesize titleView;

-(id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
    if (self)
    {
        self->pressed = false;
        self->py = 0.0f;
    }
    
    return self;
}

-(void)onStickChanged:(id)notification
{
    NSDictionary* dict = [notification userInfo];
    NSValue* vdir = [dict valueForKey:@"dir"];
    CGPoint dir = [vdir CGPointValue];

    self->pressed = !((dir.x == 0.0) && (dir.y == 0));
    NSLog(@"S: %f", dir.x, nil);
    
    Car* connectedCar = [CarsController getInstance].connectedCar;
    
    if (nil == connectedCar)
    {
        return;
    }
    
    UITabBarController* tabController = (UITabBarController*)self.view.window.rootViewController;
    
    if (tabController.selectedViewController != self)
    {
        return;
    }
    
    if (pressed == true)
    {
        float v = dir.x / 2.0f; //(1.0 - self.speed.value - 0.5);
        
        float nds = connectedCar.throttleCenter - connectedCar.throttleMin;
        float pds = connectedCar.throttleMax - connectedCar.throttleCenter;
        
        int throttle = connectedCar.throttleCenter + ((v < 0) ? (nds * v * 2) : (pds * v * 2));
        
        connectedCar.throttle = throttle;
    }
    else
    {
        connectedCar.throttle = connectedCar.throttleCenter;
    }
}

-(void)updateValues
{
    Car* connectedCar = [CarsController getInstance].connectedCar;
    
    if (nil == connectedCar)
    {
        return;
    }
    
    self.titleView.title = [NSString stringWithFormat:@"%@", connectedCar.name, nil];
    
    if (self->tempGage.temperature != connectedCar.temperature)
    {
        self->tempGage.temperature = connectedCar.temperature;
        [self->tempGage setNeedsDisplay];
    }
    
    int batteryLevel = (int)(100.0f * connectedCar.mainBatteryVoltage / connectedCar.mainBatteryMaxVoltage);
    
    if (self->fuelGage.batteryLevel != batteryLevel)
    {
        self->fuelGage.batteryLevel = batteryLevel;
        [self->fuelGage setNeedsDisplay];
    }
    
    if ((int)(self->rpmGage.rpm * 100) != (int)(connectedCar.loadCurrent * 100))
    {
        self->rpmGage.rpm = connectedCar.loadCurrent;
        [self->rpmGage setNeedsDisplay];
        
        self->speedGage.speed = connectedCar.loadCurrent;
        [self->speedGage setNeedsDisplay];
    }
    
    self.batteryLabel.transform = CGAffineTransformMakeRotation(M_PI / 2.0f);
    
    self.batteryLabel.text = [NSString stringWithFormat:@"%0.2fV %0.2fA %0.2fC (%0.2f, %0.2f, %0.2f)", connectedCar.mainBatteryVoltage, connectedCar.loadCurrent, connectedCar.temperature, connectedCar.accelerationX, connectedCar.accelerationY, connectedCar.accelerationZ];
}

-(void)viewDidLoad
{
    self.accelerometer = [UIAccelerometer sharedAccelerometer];
    self.accelerometer.updateInterval = .001;
    self.accelerometer.delegate = self;

    [super viewDidLoad];
    
    NSNotificationCenter* notificationCenter = [NSNotificationCenter defaultCenter];
    [notificationCenter addObserver: self selector: @selector(onStickChanged:) name: @"StickChanged" object: nil];
    
    self.speedGage.alpha = 0.0f;
    self.rpmGage.alpha = 0.0f;
    self.fuelGage.alpha = 0.0f;
    self.tempGage.alpha = 0.0f;
    self.leftButton.alpha = 0.0f;
    self.rightButton.alpha = 0.0f;
    self.emergencyButton.alpha = 0.0;
    self.lightsButton.alpha = 0.0f;
    self.batteryLabel.alpha = 0.0f;
    
    self.speedGage.backgroundColor = NULL;
    self.rpmGage.backgroundColor = NULL;
    self.fuelGage.backgroundColor = NULL;
    self.tempGage.backgroundColor = NULL;
    self.leftButton.backgroundColor = NULL;
    self.rightButton.backgroundColor = NULL;
    self.emergencyButton.backgroundColor = NULL;
    self.lightsButton.backgroundColor = NULL;
    
    NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateValues) userInfo:nil repeats:YES];
    [timer fire];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);;
}

-(void)accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
    if (0 == self.navigationBar.alpha)
    {
        Car* connectedCar = [CarsController getInstance].connectedCar;

        if (nil == connectedCar)
        {
            return;
        }

        UITabBarController* tabController = (UITabBarController*)self.view.window.rootViewController;
   
        if (tabController.selectedViewController != self)
        {
            return;
        }
    
        float v = acceleration.y;
        
        if (v < -0.5)
        {
            v = -0.5;
        }

        if (v > 0.5)
        {
            v = 0.5;
        }
        
        v *= -1.0;
   
        int nds = connectedCar.steeringCenter - connectedCar.steeringMin;
        int pds = connectedCar.steeringMax - connectedCar.steeringCenter;
        
        int steering = connectedCar.steeringCenter + ((v < 0) ? (nds * v * 2) : (pds * v * 2));
        
        connectedCar.steering = steering;
    }
}

-(IBAction)disconnectPressed:(id)sender
{
    [[[CarsController getInstance] connectedCar] disconnect];
}

-(IBAction)lightsPressed:(id)sender
{
	Car* connectedCar = [CarsController getInstance].connectedCar;
	
	if (nil == connectedCar)
	{
		return;
	}
	
	connectedCar.headLightsOn = !connectedCar.headLightsOn;
	connectedCar.breakLightsOn = !connectedCar.breakLightsOn;
}

-(IBAction)leftTurnPressed:(id)sender
{
	Car* connectedCar = [CarsController getInstance].connectedCar;
	
	if (nil == connectedCar)
	{
		return;
	}
	
	connectedCar.leftTurnLightsOn = !connectedCar.leftTurnLightsOn;
}

-(IBAction)rightTurnPressed:(id)sender
{
	Car* connectedCar = [CarsController getInstance].connectedCar;
	
	if (nil == connectedCar)
	{
		return;
	}
	
	connectedCar.rightTurnLightsOn = !connectedCar.rightTurnLightsOn;
}

-(IBAction)emergencyPressed:(id)sender
{
}

-(IBAction)powerPressed:(id)sender
{
    if (1 == self.navigationBar.alpha)
    {
        [UIView animateWithDuration:1.0 animations:^
        {
            self.navigationBar.alpha = 0;
            self.tabBarController.tabBar.alpha = 0;
            self.leftButton.alpha = 1;
            self.rightButton.alpha = 1;
            self.emergencyButton.alpha = 1;
            self.lightsButton.alpha = 1;
            self.throttleSlider.alpha = 1;
            self.speedGage.alpha = 1;
            self.fuelGage.alpha = 1;
            self.rpmGage.alpha = 1;
            self.tempGage.alpha = 1;
            self.batteryLabel.alpha = 1;
        
            self.powerButton.frame = CGRectMake(251, self.view.bounds.size.height / 2 + 105, self.powerButton.frame.size.width, self.powerButton.frame.size.height);
            
            self.statusBarView.alpha = 0;
        }];

        [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationSlide];
    }
    else
    {
        [UIView animateWithDuration:1.0 animations:^
        {
            self.navigationBar.alpha = 1;
            self.tabBarController.tabBar.alpha = 1;
            self.leftButton.alpha = 0;
            self.rightButton.alpha = 0;
            self.emergencyButton.alpha = 0;
            self.lightsButton.alpha = 0;
            self.throttleSlider.alpha = 0;
            self.speedGage.alpha = 0;
            self.fuelGage.alpha = 0;
            self.rpmGage.alpha = 0;
            self.tempGage.alpha = 0;
            self.statusBarView.alpha = 1;
            self.batteryLabel.text = 0;
        
            self.powerButton.frame = CGRectMake(self.view.frame.size.width/2 - self.powerButton.frame.size.width/2, self.view.frame.size.height/2 - self.powerButton.frame.size.height/2, self.powerButton.frame.size.width, self.powerButton.frame.size.height);
        }];

        [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationSlide];
    }
}

@end
