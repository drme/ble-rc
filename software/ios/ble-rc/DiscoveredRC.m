#import "DiscoveredRC.h"

@implementation DiscoveredRC

@synthesize deviceName;
@synthesize deviceAddress;
@synthesize device;

-(id)initWithData:(CBPeripheral*)peripheral;
{
    self = [super init];
    
    if (self)
    {
        self.device = peripheral;
        self.deviceName = self.device.name;
        
        
        if ([self.deviceName isEqual:@""])
        {
            self.deviceName = @"Buggy";
        }
        else if (self.deviceName == nil)
        {
            self.deviceName = @"Buggy";
        }
        
        //if (nil != self.device.identifier)
        //{
        //    self.deviceAddress = [NSString stringWithFormat:@"%@", device.identifier.UUIDString];
        //}
    }
    
    return self;
}

-(id)initWithFakeData:(NSString*)name
{
    self = [super init];
    
    if (self)
    {
        self.device = NULL;
        self.deviceName = name;
    }
    
    return self;
}

@end
