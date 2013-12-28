#import <UIKit/UIKit.h>
#import "CarsController.h"

@interface ConnectViewController : UIViewController<UITableViewDataSource, UITableViewDelegate, CarsSearchDelegate, UITabBarControllerDelegate>
{
    IBOutlet UITableView* tableView;
}

-(IBAction)searchClicked:(id)sender;
-(IBAction)connectClicked:(id)sender;

@end
