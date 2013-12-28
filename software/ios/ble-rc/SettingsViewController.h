#import <UIKit/UIKit.h>

@interface SettingsViewController : UIViewController

@property (weak, nonatomic) IBOutlet UITextField* steeringMinValue;
@property (weak, nonatomic) IBOutlet UITextField* steeringMaxValue;
@property (weak, nonatomic) IBOutlet UITextField* steeringCenterValue;

@property (weak, nonatomic) IBOutlet UITextField* throttleMinValue;
@property (weak, nonatomic) IBOutlet UITextField* throttleMaxValue;
@property (weak, nonatomic) IBOutlet UITextField* throttleCenterValue;

@property (weak, nonatomic) IBOutlet UISlider* steeringMinSlider;
@property (weak, nonatomic) IBOutlet UISlider* steeringMaxSlider;
@property (weak, nonatomic) IBOutlet UISlider* steeringCenterSlider;

@property (weak, nonatomic) IBOutlet UISlider* throttleMinSlider;
@property (weak, nonatomic) IBOutlet UISlider* throttleMaxSlider;
@property (weak, nonatomic) IBOutlet UISlider* throttleCenterSlider;

@property (weak, nonatomic) IBOutlet UITextField* batteryVoltageValue;
@property (weak, nonatomic) IBOutlet UITextField* batteryCapacityValue;

@property (weak, nonatomic) IBOutlet UITextField* carNameValue;

@property (weak, nonatomic) IBOutlet UINavigationItem* titleView;

-(IBAction)textValueChanged:(id)sender;
-(IBAction)valueChanged:(id)sender;
-(IBAction)sliderMoved:(id)sender;
-(IBAction)sliderReleased:(id)sender;
-(IBAction)refreshSelected:(id)sender;
-(IBAction)helpSelected:(id)sender;

@end
