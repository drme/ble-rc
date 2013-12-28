#import "SettingsViewController.h"
#import "CarsController.h"

@implementation SettingsViewController

@synthesize steeringMinValue;
@synthesize steeringMaxValue;
@synthesize steeringCenterValue;
@synthesize throttleMinValue;
@synthesize throttleMaxValue;
@synthesize throttleCenterValue;

@synthesize steeringMinSlider;
@synthesize steeringMaxSlider;
@synthesize steeringCenterSlider;

@synthesize throttleMinSlider;
@synthesize throttleMaxSlider;
@synthesize throttleCenterSlider;

@synthesize batteryVoltageValue;
@synthesize batteryCapacityValue;

@synthesize titleView;

-(id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    if (self)
    {
    }

    return self;
}

-(void)fillData
{
    Car* c = [CarsController getInstance].connectedCar;
    
    if (nil == c)
    {
        return;
    }
    
    self.steeringMinValue.text = [NSString stringWithFormat:@"%d", c.steeringMin];
    self.steeringMaxValue.text = [NSString stringWithFormat:@"%d", c.steeringMax];
    self.steeringCenterValue.text = [NSString stringWithFormat:@"%d", c.steeringCenter];
    self.throttleMinValue.text = [NSString stringWithFormat:@"%d", c.throttleMin];
    self.throttleMaxValue.text = [NSString stringWithFormat:@"%d", c.throttleMax];
    self.throttleCenterValue.text = [NSString stringWithFormat:@"%d", c.throttleCenter];
    
    self.steeringCenterSlider.value = c.steeringCenter;
    self.steeringMaxSlider.value = c.steeringMax;
    self.steeringMinSlider.value = c.steeringMin;
    
    self.throttleCenterSlider.value = c.throttleCenter;
    self.throttleMaxSlider.value = c.throttleMax;
    self.throttleMinSlider.value = c.throttleMin;
    
    self.carNameValue.text = c.name;
    self.batteryVoltageValue.text = [NSString stringWithFormat:@"%0.2f", (float)c.mainBatteryMaxVoltage];
    self.batteryCapacityValue.text = [NSString stringWithFormat:@"%d", c.mainBatteryCapacity];
    
    self.titleView.title = [NSString stringWithFormat:@"%@", c.name, nil];
}

-(IBAction)refreshSelected:(id)sender
{
    [self fillData];
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    [self.view addGestureRecognizer:[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)]];
    
    [self fillData];
}


-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

-(void)dismissKeyboard
{
 //   [self.throttleValue resignFirstResponder];
 //   [self.wheelValue resignFirstResponder];
 //   [self.cycleValue resignFirstResponder];
 //   [self.steeringMinValue resignFirstResponder];
 //   [self.steeringMaxValue resignFirstResponder];
 //   [self.steeringCenterValue resignFirstResponder];
 //   [self.throttleMinValue resignFirstResponder];
 //   [self.throttleMaxValue resignFirstResponder];
 //   [self.throttleCenterValue resignFirstResponder];
}

-(IBAction)valueChanged:(id)sender
{
    Car* c = [CarsController getInstance].connectedCar;
    
    if (nil == c)
    {
        return;
    }
    
    UISlider* slider = sender;
    
    if (sender == self.throttleCenterSlider)
    {
        self.throttleCenterValue.text = [NSString stringWithFormat:@"%d", (int)slider.value];
    }
    else if (sender == self.throttleMinSlider)
    {
        self.throttleMinValue.text = [NSString stringWithFormat:@"%d", (int)slider.value];
    }
    else if (sender == self.throttleMaxSlider)
    {
        self.throttleMaxValue.text = [NSString stringWithFormat:@"%d", (int)slider.value];
    }
    else if (sender == self.steeringCenterSlider)
    {
        self.steeringCenterValue.text = [NSString stringWithFormat:@"%d", (int)slider.value];
    }
    else if (sender == self.steeringMinSlider)
    {
        self.steeringMinValue.text = [NSString stringWithFormat:@"%d", (int)slider.value];
    }
    else if (sender == self.steeringMaxSlider)
    {
        self.steeringMaxValue.text = [NSString stringWithFormat:@"%d", (int)slider.value];
    }
}

-(IBAction)textValueChanged:(id)sender
{
    Car* c = [CarsController getInstance].connectedCar;
    
    if (nil == c)
    {
        //       return;
    }
    
    UITextField* textField = sender;
    
    if (sender == self.carNameValue)
    {
        c.name = textField.text;
    }
    
    int value = [textField.text integerValue];
    
    if (sender == self.throttleMinValue)
    {
        c.throttleMin = value;
    }
    else if (sender == self.throttleMaxValue)
    {
        c.throttleMax = value;
    }
    else if (sender == self.throttleCenterValue)
    {
        c.throttleCenter = value;
    }
    if (sender == self.steeringMinValue)
    {
        c.steeringMin = value;
    }
    else if (sender == self.steeringMaxValue)
    {
        c.steeringMax = value;
    }
    else if (sender == self.steeringCenterValue)
    {
        c.steeringCenter = value;
    }
    else if (sender == self.batteryCapacityValue)
    {
        c.mainBatteryCapacity = value;
    }
    
    else if (sender == self.batteryVoltageValue)
    {
        c.mainBatteryMaxVoltage = value;
    }
}


-(IBAction)sliderMoved:(id)sender
{
    Car* car = [CarsController getInstance].connectedCar;
    
    if (nil == car)
    {
        return;
    }
    
    UISlider* slider = sender;
    int value = slider.value;
    
    if ((sender == self.steeringCenterSlider) || (sender == self.steeringMaxSlider) || (sender == self.steeringMinSlider))
    {
        car.steering = value;
    }
    
    if ((sender == self.throttleCenterSlider) || (sender == self.throttleMaxSlider) || (sender == self.throttleMinSlider))
    {
        car.throttle = value;
    }
}

-(IBAction)sliderReleased:(id)sender
{
    Car* car = [CarsController getInstance].connectedCar;
    
    if (nil == car)
    {
        return;
    }
    
    UISlider* slider = sender;
    int value = slider.value;
    
    if (sender == self.steeringCenterSlider)
    {
        car.steeringCenter = value;
    }
    else if (sender == self.steeringMaxSlider)
    {
        car.steeringMax = value;
    }
    else if (sender == self.steeringMinSlider)
    {
        car.steeringMin = value;
    }
    if (sender == self.throttleCenterSlider)
    {
        car.throttleCenter = value;
    }
    else if (sender == self.throttleMaxSlider)
    {
        car.throttleMax = value;
    }
    else if (sender == self.throttleMinSlider)
    {
        car.throttleMin = value;
    }
   
    if ((sender == self.steeringCenterSlider) || (sender == self.steeringMaxSlider) || (sender == self.steeringMinSlider))
    {
        car.steering = car.steeringCenter;
    }

    if ((sender == self.throttleCenterSlider) || (sender == self.throttleMaxSlider) || (sender == self.throttleMinSlider))
    {
        car.throttle = car.throttleCenter;
    }
}

-(IBAction)helpSelected:(id)sender
{
    UIAlertView* helpAlert = [[UIAlertView alloc] initWithTitle:@"Adjusting controlls" message:@"Throttle:\nAdjust the center value till the car is not moving.\nAdjust the min value till the car goes at max backward speed.\nAdjust the max value till the car goes at the max forward speed.\n\nSteering:\nAdjust the center value till the wheels are centered straight.\nAdjust the min value till the wheels are fully rotated to the left.\nAdjust the max value till the wheels are fully rotated to the right." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];

    [helpAlert show];
}

@end
