#import "StatusViewController.h"
#import "CarsController.h"

@implementation StatusViewController

@synthesize temperatureTextField;
@synthesize mainBatteryVoltageTextField;
@synthesize mainBatteryVoltagePercentTextField;
@synthesize controllerBatteryVoltageField;
@synthesize controllerBatteryVoltagePercentField;
@synthesize currentConsumptionField;
@synthesize mainBatteryProgressView;
@synthesize controllerBatteryProgressView;
@synthesize temperatureProgressView;
@synthesize currentConsumptionProgressView;
@synthesize titleView;

-(id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    if (self)
    {
    }

    return self;
}

-(float)voltageToCapacityAlkaline15V:(float)voltage
{
    if (voltage >= 1.25f)
    {
        //1.5 - 1;
        //1.25 - 0.75;
        
        return 1.0 - (((1 - 0.75) / (1.5 - 1.25)) * (1.5 - voltage));
    }
    else if (voltage <= 1.05f)
    {
        //1.05 - 0.18
        //0 - 0
        
        return 0.18 - (((0.18 - 0.00) / (1.05 - 0.00)) * (1.05 - voltage));
    }
    else
    {
        //1.25 - 0.75;
        //1.05 - 0.18
        
        return 0.75 - (((0.75 - 0.18) / (1.25 - 1.05)) * (1.25 - voltage));
    }
}

-(void)updateValues
{
    Car* connectedCar = [CarsController getInstance].connectedCar;
    
    if (nil != connectedCar)
    {
        self.titleView.title = [NSString stringWithFormat:@"%@", connectedCar.name, nil];
        self.temperatureTextField.text = [NSString stringWithFormat:@"%0.2f C", (connectedCar.temperature / 1.0f)];
        self.temperatureProgressView.progress = connectedCar.temperature / 100.0f;
        self.mainBatteryVoltageTextField.text = [NSString stringWithFormat:@"%0.2f V", connectedCar.mainBatteryVoltage];
        self.mainBatteryVoltagePercentTextField.text = [NSString stringWithFormat:@"%d%%", (int)(100 * connectedCar.mainBatteryVoltage / connectedCar.mainBatteryMaxVoltage)];
        self.mainBatteryProgressView.progress = connectedCar.mainBatteryVoltage / connectedCar.mainBatteryMaxVoltage;
        self.controllerBatteryVoltageField.text = [NSString stringWithFormat:@"%0.2f V", (2.0 + connectedCar.controllerBatteryLevel / 100.0f)];
        self.controllerBatteryVoltagePercentField.text = [NSString stringWithFormat:@"%d%%", (int)(connectedCar.controllerBatteryLevel)];
        self.controllerBatteryProgressView.progress = connectedCar.controllerBatteryLevel / 100.0f;;
        self.currentConsumptionField.text = [NSString stringWithFormat:@"%0.2f A", connectedCar.loadCurrent];
        self.currentConsumptionProgressView.progress = connectedCar.loadCurrent / 20.0f;
    }
    else
    {
        self.titleView.title = @"Disconnected";
        self.temperatureTextField.text = @"0 C";
        self.temperatureProgressView.progress = 0.0f;
        self.mainBatteryVoltageTextField.text = @"0.0 V";
        self.mainBatteryVoltagePercentTextField.text = @"0 %";
        self.mainBatteryProgressView.progress = 0.0f;
        self.controllerBatteryVoltageField.text = @"0.0 V";
        self.controllerBatteryVoltagePercentField.text = @"0 %";
        self.controllerBatteryProgressView.progress = 0.0f;
        self.currentConsumptionField.text = @"0.0 A";
        self.currentConsumptionProgressView.progress = 0.0f;
    }
}

-(void)viewDidLoad
{
    [super viewDidLoad];

    NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateValues) userInfo:nil repeats:YES];
    
    [timer fire];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


@end
