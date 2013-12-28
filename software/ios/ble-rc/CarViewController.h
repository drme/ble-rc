#import <UIKit/UIKit.h>
#import "JoyStickView.h"
#import "FuelGage.h"
#import "SpeedGage.h"
#import "RPMGage.h"
#import "TempGage.h"

@interface CarViewController : UIViewController<UIAccelerometerDelegate>

@property (nonatomic, retain) UIAccelerometer *accelerometer;

@property (weak, nonatomic) IBOutlet UINavigationBar* navigationBar;
@property (weak, nonatomic) IBOutlet UITabBar* tabBar;
@property (weak, nonatomic) IBOutlet UIButton* powerButton;
@property (weak, nonatomic) IBOutlet UIButton* leftButton;
@property (weak, nonatomic) IBOutlet UIButton* rightButton;
@property (weak, nonatomic) IBOutlet UIButton* emergencyButton;
@property (weak, nonatomic) IBOutlet UIButton* lightsButton;
@property (weak, nonatomic) IBOutlet JoyStickView* throttleSlider;
@property (weak, nonatomic) IBOutlet TempGage* tempGage;
@property (weak, nonatomic) IBOutlet FuelGage* fuelGage;
@property (weak, nonatomic) IBOutlet SpeedGage* speedGage;
@property (weak, nonatomic) IBOutlet RPMGage* rpmGage;
@property (weak, nonatomic) IBOutlet UIView* statusBarView;
@property (weak, nonatomic) IBOutlet UILabel* batteryLabel;
@property (weak, nonatomic) IBOutlet UINavigationItem* titleView;

-(IBAction)disconnectPressed:(id)sender;
-(IBAction)lightsPressed:(id)sender;
-(IBAction)leftTurnPressed:(id)sender; 
-(IBAction)rightTurnPressed:(id)sender; 
-(IBAction)emergencyPressed:(id)sender;
-(IBAction)powerPressed:(id)sender;

@end
