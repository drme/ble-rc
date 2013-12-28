#import "CarsController.h"
#import "Utils.h"
#import "Car.h"

@implementation CarsController
{
    CBCentralManager* centralManager;
    NSMutableArray* peripherals;
}

@synthesize delegateDiscover;
@synthesize connectedCar;

-(id)init
{
    self = [super init];
    
    if (self)
    {
        self->centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    }
    
    return self;
}

+(CarsController*)getInstance
{
    static CarsController* sharedSingleton;
    
    @synchronized(self)
    {
        if (!sharedSingleton)
        {
            sharedSingleton = [[CarsController alloc] init];
        }
        
        return sharedSingleton;
    }
}

-(void)startDeviceSearch
{
    [self->peripherals removeAllObjects];
    
    int serviceUUID = CAR_SERVICE_UUID;
    UInt16 c = [Utils swap:serviceUUID];
    NSData* cd = [[NSData alloc] initWithBytes:(char *)&c length:2];
    
    if (CBCentralManagerStatePoweredOn != self->centralManager.state)
    {
        //[Utils showMessage:@"CoreBluetooth not correctly initialized !"];
        
        NSLog(@"CoreBluetooth not correctly initialized!");
        NSLog(@"State = %d (%s)", self->centralManager.state, [CarsController centralManagerStateToString:self->centralManager.state]);
        
        [self.delegateDiscover searchEnded];
        
        return;
    }
    
    [NSTimer scheduledTimerWithTimeInterval:(float)5.0 target:self selector:@selector(scanTimer:) userInfo:nil repeats:NO];
    
    NSDictionary* options = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], CBCentralManagerScanOptionAllowDuplicatesKey, nil];

    NSArray* services = [NSArray arrayWithObject:[CBUUID UUIDWithData:cd]];
    
    [self->centralManager scanForPeripheralsWithServices:services options:options];
}

-(void)connect:(DiscoveredRC*)device
{
    NSLog(@"Connecting to peripheral with UUID : %@", device.device.identifier.UUIDString);

    [self->centralManager connectPeripheral:device.device options:nil];
}

-(void)connected
{
    [self->delegateDiscover connected];
}

-(void)disconnected
{
    [self->delegateDiscover disconnected];
}

+(const char*)centralManagerStateToString:(int)state
{
    switch(state)
    {
        case CBCentralManagerStateUnknown: 
            return "State unknown (CBCentralManagerStateUnknown)";
        case CBCentralManagerStateResetting:
            return "State resetting (CBCentralManagerStateUnknown)";
        case CBCentralManagerStateUnsupported:
            return "State BLE unsupported (CBCentralManagerStateResetting)";
        case CBCentralManagerStateUnauthorized:
            return "State unauthorized (CBCentralManagerStateUnauthorized)";
        case CBCentralManagerStatePoweredOff:
            return "State BLE powered off (CBCentralManagerStatePoweredOff)";
        case CBCentralManagerStatePoweredOn:
            return "State powered up and ready (CBCentralManagerStatePoweredOn)";
        default:
            return "State unknown";
    }
    
    return "Unknown state";
}

-(void)printKnownPeripherals
{
    NSLog(@"Known peripherals: %d", [self->peripherals count]);
    NSLog(@"List of currently known peripherals:");

    for (int i = 0; i < self->peripherals.count; i++)
    {
        CBPeripheral* p = [self->peripherals objectAtIndex:i];
        NSLog(@"%d  |  %@", i, p.identifier.UUIDString);
    }
}

-(void)scanTimer:(NSTimer*)timer
{
    [self->centralManager stopScan];
    [self.delegateDiscover searchEnded];
    [self printKnownPeripherals];
}


- (int) UUIDSAreEqual:(CFUUIDRef)u1 u2:(CFUUIDRef)u2 {
    
    if ((nil == u1) && (nil == u2))
    {
        return 0;
    }
    else if (((nil == u1) && (nil != u2)) || ((nil != u1) && (nil == u2)))
    {
        return 1;
    }
    
    
    CFUUIDBytes b1 = CFUUIDGetUUIDBytes(u1);
    CFUUIDBytes b2 = CFUUIDGetUUIDBytes(u2);
    if (memcmp(&b1, &b2, 16) == 0) {
        return 1;
    }
    else return 0;
}

-(const char *) UUIDToString:(CFUUIDRef)UUID {
    if (!UUID) return "NULL";
    CFStringRef s = CFUUIDCreateString(NULL, UUID);
    return CFStringGetCStringPtr(s, 0);		
    
}


-(void)centralManagerDidUpdateState:(CBCentralManager*)central
{
    NSLog(@"Status of CoreBluetooth central manager changed %d (%s)", central.state, [CarsController centralManagerStateToString:central.state]);
}

-(void)centralManager:(CBCentralManager*)central didRetrievePeripherals:(NSArray*)peripherals
{
}

-(void)centralManager:(CBCentralManager*)central didRetrieveConnectedPeripherals:(NSArray*)peripherals
{
}

-(void)centralManager:(CBCentralManager*)central didDiscoverPeripheral:(CBPeripheral*)peripheral advertisementData:(NSDictionary*)advertisementData RSSI:(NSNumber*)RSSI
{
    NSLog(@"Received periferal: %@", peripheral);
    
    NSLog(@"Advertisement data: %@", advertisementData);
    
    if (nil == self->peripherals)
    {
        self->peripherals = [[NSMutableArray alloc] initWithObjects:peripheral, nil];

        [self->delegateDiscover deviceFound:[[DiscoveredRC alloc] initWithData:peripheral]];
    }
    else
    {
        for(int i = 0; i < self->peripherals.count; i++)
        {
            CBPeripheral* p = [self->peripherals objectAtIndex:i];
            
            if ([self UUIDSAreEqual:p.UUID u2:peripheral.UUID])
            {
                [self->peripherals replaceObjectAtIndex:i withObject:peripheral];
                
                NSLog(@"Duplicate UUID found updating...");
                
                return;
            }
        }
        
        [self->peripherals addObject:peripheral];
        
        [self->delegateDiscover deviceFound:[[DiscoveredRC alloc] initWithData:peripheral]];
        
        NSLog(@"New UUID, adding");
    }
    
    NSLog(@"didDiscoverPeripheral");
}

-(void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral
{
    NSLog(@"Connection to peripheral with UUID : %@ successfull.", peripheral.identifier.UUIDString);

    self.connectedCar = [[Car alloc] initWithData:peripheral:self];
}

-(void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error
{
    NSLog(@"Failure to connect");
}

-(void)centralManager:(CBCentralManager*)central didDisconnectPeripheral:(CBPeripheral*)peripheral error:(NSError*)error
{
    printf("Disconnect\n");
    
    [self.connectedCar disconnect];
    
    self.connectedCar = nil;
    
    [self disconnected];
    
    [Utils showMessage:@"Disconnected"];
}

-(void)disconnectActive:(CBPeripheral*)device
{
    [self->centralManager cancelPeripheralConnection:device];
    self.connectedCar = nil;
}

@end
