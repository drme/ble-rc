#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreBluetooth/CBService.h>

@interface DiscoveredRC : NSObject

@property (assign) CBPeripheral* device;
@property (assign) NSString* deviceName;
@property (assign) NSString* deviceAddress;

-(id)initWithData:(CBPeripheral*)peripheral;
-(id)initWithFakeData:(NSString*)name;

@end
