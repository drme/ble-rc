#import "DiscoveredRC.h"

@implementation DiscoveredRC

@synthesize deviceName;
@synthesize deviceAddress;
@synthesize device;
@synthesize connectable;
@synthesize comment;

-(id)initWithData:(CBPeripheral*)peripheral;
{
	self = [super init];

	if (self)
	{
		self.device = peripheral;
		self.deviceName = self.device.name;
		self.deviceAddress = peripheral.identifier;
		
		if (self.deviceName == nil)
		{
			self.deviceName = @"Buggy";
		}
		else if ([self.deviceName isEqual:@""])
		{
			self.deviceName = @"Buggy";
		}

		self.connectable = true;
	}
    
	return self;
};

-(id)initWithFakeData:(NSString*)name :(bool)canConnect
{
	self = [super init];

	if (self)
	{
		self.device = NULL;
		self.deviceName = name;
		self.connectable = canConnect;
	}
    
	return self;
};

@end
