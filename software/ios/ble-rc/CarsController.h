#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreBluetooth/CBService.h>
#import "DiscoveredRC.h"
#import "Car.h"

@protocol CarsSearchDelegate

-(void)deviceFound:(DiscoveredRC*)device;
-(void)searchEnded;
-(void)connected;
-(void)disconnected;

@end

@interface CarsController : NSObject<CBCentralManagerDelegate, CarDelegate>

@property (nonatomic,assign) id<CarsSearchDelegate> delegateDiscover;
@property (strong, nonatomic) Car* connectedCar;

-(void)startDeviceSearch;
-(void)connect:(DiscoveredRC*)device;
+(CarsController*)getInstance;
+(const char*)centralManagerStateToString:(int)state;
-(void)disconnectActive:(CBPeripheral*)device;

@end
