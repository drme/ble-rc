#import <UIKit/UIKit.h>

@interface StatusViewController : UIViewController

@property (weak, nonatomic) IBOutlet UITextField* temperatureTextField;
@property (weak, nonatomic) IBOutlet UITextField* mainBatteryVoltageTextField;
@property (weak, nonatomic) IBOutlet UITextField* mainBatteryVoltagePercentTextField;
@property (weak, nonatomic) IBOutlet UITextField* controllerBatteryVoltageField;
@property (weak, nonatomic) IBOutlet UITextField* controllerBatteryVoltagePercentField;
@property (weak, nonatomic) IBOutlet UITextField* currentConsumptionField;
@property (weak, nonatomic) IBOutlet UIProgressView* mainBatteryProgressView;
@property (weak, nonatomic) IBOutlet UIProgressView* controllerBatteryProgressView;
@property (weak, nonatomic) IBOutlet UIProgressView* temperatureProgressView;
@property (weak, nonatomic) IBOutlet UIProgressView* currentConsumptionProgressView;
@property (weak, nonatomic) IBOutlet UINavigationItem* titleView;

@end
