#import "ConnectViewController.h"
#import "DiscoveredRC.h"
#import "CarsController.h"
#import "SettingsViewController.h"
#import "CarViewController.h"
#import "StatusViewController.h"

@implementation ConnectViewController
{
    UIAlertView* searchDialog;
    UIAlertView* connectDialog;
    NSMutableArray* devices;
    bool searchEnabled;
}

-(id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];

    if (self)
    {
        self->devices = [[NSMutableArray alloc] init];
    }
    
    return self;
}

-(void)viewDidLoad
{
    [super viewDidLoad];
    
    [self searchClicked:self];
    
    [self.tabBarController setDelegate:self];
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

-(IBAction)searchClicked:(id)sender
{
    if (nil == self->searchDialog)
    {
        self->searchDialog = [[UIAlertView alloc]initWithTitle:@"Search" message:@"Searching for RC cars" delegate:nil cancelButtonTitle:@"Hide" otherButtonTitles:nil];

        [self->searchDialog show];
    
        /* if (nil != self->searchDialog)
        {
            UIActivityIndicatorView* indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        
            indicator.center = CGPointMake(self->searchDialog.bounds.size.width / 2, self->searchDialog.bounds.size.height - 45);
            
            [indicator startAnimating];
            
            [self->searchDialog addSubview:indicator];
        } */
        
        //UIActivityIndicatorView* indicator = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(0, 0, 20, 20)];
        //UIBarButtonItem* barButton = [[UIBarButtonItem alloc] initWithCustomView:indicator];
        //self.navigationItem.rightBarButtonItem = barButton;
        //[indicator startAnimating];
    }
    
    //if (nil == self->devices)
    {
        self->devices = [[NSMutableArray alloc] init];
    }
    
    [CarsController getInstance].delegateDiscover = self;
    [[CarsController getInstance] startDeviceSearch];
    
    [self->tableView reloadData];
}

-(IBAction)connectClicked:(id)sender
{
 //   [self deviceFound:[[DiscoveredRC alloc] initWithFakeData:@"beemmer"]];
 //   [self deviceFound:[[DiscoveredRC alloc] initWithFakeData:@"Italia"]];
 //   [self deviceFound:[[DiscoveredRC alloc] initWithFakeData:@"Drifter"]];
 //   [self deviceFound:[[DiscoveredRC alloc] initWithFakeData:@"Buggy"]];
 //   [self deviceFound:[[DiscoveredRC alloc] initWithFakeData:@"BLE-RC"]];
    
    NSIndexPath* selectedIndexPath = [self->tableView indexPathForSelectedRow];

    if (nil != selectedIndexPath)
    {
        if ((selectedIndexPath.row >= 0) && (selectedIndexPath.row < self->devices.count))
        {
            DiscoveredRC* device = [self->devices objectAtIndex:selectedIndexPath.row];
            
            if (nil != device)
            {
                if (nil == self->searchDialog)
                {
                    self->connectDialog = [[UIAlertView alloc]initWithTitle:@"Connect" message:@"Connecting to RC..." delegate:nil cancelButtonTitle:nil otherButtonTitles:nil];
                    
                    [self->connectDialog show];
                    
                    if (nil != self->connectDialog)
                    {
                        UIActivityIndicatorView* indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
                        
                        indicator.center = CGPointMake(self->connectDialog.bounds.size.width / 2, self->searchDialog.bounds.size.height - 45);
                        
                        [indicator startAnimating];
                        
                        [self->connectDialog addSubview:indicator];
                    }
                }
                
                [[CarsController getInstance] connect:device];
            }
        }
    }
    
    NSLog(@"Connect");
}

-(void)deviceFound:(DiscoveredRC*)device
{
    [self->devices addObject:device];
    [self->tableView reloadData];
}

-(void)searchEnded
{
    [self->tableView reloadData];
    [self->searchDialog dismissWithClickedButtonIndex:0 animated:true];
    self->searchDialog = nil;
    
    if (true == self->searchEnabled)
    {
        //[self searchClicked:self];
    }
}

-(void)connected
{
    [self->connectDialog dismissWithClickedButtonIndex:0 animated:true];
    self->connectDialog = nil;
    self.tabBarController.selectedIndex = 1;
    
    self->searchEnabled = false;
}

-(void)disconnected
{
    [self->connectDialog dismissWithClickedButtonIndex:0 animated:true];
    self->connectDialog = nil;
    self.tabBarController.selectedIndex = 0;
    
    //self->searchEnabled = true;
    
    //[self searchEnded];
}

-(NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

-(NSString*)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return @"";
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self->devices.count;
}

-(UITableViewCell*)tableView:(UITableView*)tableView1 cellForRowAtIndexPath:(NSIndexPath*)indexPath
{
    DiscoveredRC* item = [self->devices objectAtIndex:indexPath.row];
    
    static NSString* cellIdentifier = @"ItemCell";
    
    UITableViewCell* cell = [tableView1 dequeueReusableCellWithIdentifier:cellIdentifier];
    
    if (cell == nil)
    {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:cellIdentifier];
    }
    
    if (nil != item.deviceName)
    {
        cell.textLabel.text = item.deviceName;
    }
    
    if (nil != item.deviceAddress)
    {
        cell.detailTextLabel.text = item.deviceAddress;
    }
    
    cell.accessoryType = UITableViewCellAccessoryNone;
    cell.selectionStyle = UITableViewCellSelectionStyleGray;
    
    return cell;
}

-(void)tableView:(UITableView*)tableView1 didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
}

-(BOOL)tableView:(UITableView*)tableView1 canEditRowAtIndexPath:(NSIndexPath*)indexPath
{
    return YES;
}

-(void)tableView:(UITableView*)tableView1 commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath*)indexPath
{
}

-(BOOL)tabBarController:(UITabBarController*)tabBarController shouldSelectViewController:(UIViewController*)viewController
{
    bool connected = (nil != [CarsController getInstance].connectedCar);
    
    if (viewController == self)
    {
        return !connected;
    }
    else if (viewController.class == SettingsViewController.class)
    {
        return connected;
    }
    else if (viewController.class == StatusViewController.class)
    {
        return connected;
    }
    else if (viewController.class == CarViewController.class)
    {
        return connected;
    }
    
    return true;
}

@end
