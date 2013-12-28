#import "Car.h"
#import "Utils.h"
#import "CarsController.h"

@implementation Car
{
    CBPeripheral* device;
	uint16_t lightsState;
    NSTimer* timer;
    CBCharacteristic* throttleCharacteristics;
    CBCharacteristic* steeringCharacteristics;
    CBCharacteristic* pulseWidthCharacteristics;
    CBCharacteristic* nameCharacteristics;
    CBCharacteristic* minThrottleCharacteristics;
    CBCharacteristic* maxThrottleCharacteristics;
    CBCharacteristic* centerThrottleCharacteristics;
    CBCharacteristic* minSteeringCharacteristics;
    CBCharacteristic* maxSteeringCharacteristics;
    CBCharacteristic* centerSteeringCharacteristics;
    CBCharacteristic* batteryVoltageCharacteristics;
    CBCharacteristic* batteryCurrentCharacteristics;
    CBCharacteristic* batteryMaxVoltageCharacteristics;
    CBCharacteristic* batteryCapacityCharacteristics;
    CBCharacteristic* temperatureCharacteristics;
    CBCharacteristic* controllerBatteryVoltageCharacteristics;
	CBCharacteristic* lightsCharacteristics;
    CBCharacteristic* accelerometerEnableCharacteristics;
    CBCharacteristic* accelerometerValueCharacteristics;
    
    NSString* nameValue;
    int steeringMinValue;
    int steeringMaxValue;
    int steeringCenterValue;
    int throttleMinValue;
    int throttleMaxValue;
    int throttleCenterValue;
    int steeringValue;
    int throttleValue;
    int maxBatteryVoltageValue;
    int maxBatteryCapacityValue;
}

@synthesize delegateConnect;
@synthesize temperature;
@synthesize mainBatteryVoltage;
@synthesize controllerBatteryLevel;
@synthesize accelerationX;
@synthesize accelerationY;
@synthesize accelerationZ;

-(NSString*)getName
{
    return self->nameValue;
}

-(void)setName:(NSString*)value
{
    if (nil != self->nameCharacteristics)
    {
        self->nameValue = value;
    
        uint8_t deviceName[20] = { 0 };
    
        for (int i = 0; i < self->nameValue.length && i < 20; i++)
        {
            deviceName[i] = [self->nameValue characterAtIndex:i];
        }

        NSData* data = [[NSData alloc] initWithBytes:deviceName length:20];
    
        [self->device writeValue:data forCharacteristic:self->nameCharacteristics type:CBCharacteristicWriteWithoutResponse];
    }
}

-(void)setCharacteristics:(CBCharacteristic*)characteristics1 :(int)value
{
    if (nil != characteristics1)
    {
        uint16_t v2 = value;
    
        NSData* data = [[NSData alloc] initWithBytes:&v2 length:2];
    
        [self->device writeValue:data forCharacteristic:characteristics1 type:CBCharacteristicWriteWithoutResponse];
    }
}

-(void)enableAccelerometer:(boolean_t)enable
{
    if (nil != self->accelerometerEnableCharacteristics)
    {
        uint8_t configurationData = (true == enable) ? 1 : 0;
    
        NSData* data = [[NSData alloc] initWithBytes:&configurationData length:1];
    
        [self->device writeValue:data forCharacteristic:self->accelerometerEnableCharacteristics type:CBCharacteristicWriteWithResponse];
    }
}

-(void)setSteeringMax:(int)value
{
    self->steeringMaxValue = value;
    [self setCharacteristics:self->maxSteeringCharacteristics:value];
}

-(void)setSteeringMin:(int)value
{
    self->steeringMinValue = value;
    [self setCharacteristics:self->minSteeringCharacteristics:value];
}

-(void)setSteeringCenter:(int)value
{
    self->steeringCenterValue = value;
    [self setCharacteristics:self->centerSteeringCharacteristics:value];
}

-(int)getSteeringMax
{
    return self->steeringMaxValue;
}

-(int)getSteeringMin
{
    return self->steeringMinValue;
}

-(int)getSteeringCenter
{
    return self->steeringCenterValue;
}

-(void)setThrottleMax:(int)value
{
    self->throttleMaxValue = value;
    [self setCharacteristics:self->maxThrottleCharacteristics:value];
}

-(void)setThrottleMin:(int)value
{
    self->throttleMinValue = value;
    [self setCharacteristics:self->minThrottleCharacteristics:value];
}

-(void)setThrottleCenter:(int)value
{
    self->throttleCenterValue = value;
    [self setCharacteristics:self->centerThrottleCharacteristics:value];
}

-(int)getThrottleMax
{
    return self->throttleMaxValue;
}

-(int)getThrottleMin
{
    return self->throttleMinValue;
}

-(int)getThrottleCenter
{
    return self->throttleCenterValue;
}

-(int)getMainBatteryMaxVoltage
{
    return self->maxBatteryVoltageValue;
}

-(void)setMainBatteryMaxVoltage:(int)value
{
    self->maxBatteryVoltageValue = value;
    [self setCharacteristics:self->batteryMaxVoltageCharacteristics:value];
}

-(int)getMainBatteryCapacity
{
    return self->maxBatteryCapacityValue;
}

-(void)setMainBatteryCapacity:(int)value
{
    self->maxBatteryCapacityValue= value;
    [self setCharacteristics:self->batteryCapacityCharacteristics:value];
}

-(void)updateLights:(bool)value :(int)bit
{
	if (value)
	{
		uint16_t v = 1 << bit;
		
		self->lightsState |= v;
	}
	else
	{
		uint16_t v = 1 << bit;
		
 		self->lightsState &= ~v;
	}
	
    uint16_t v2 = self->lightsState;
	
	NSData *d2 = [[NSData alloc] initWithBytes:&v2 length:2];   
	
	[device writeValue:d2 forCharacteristic:self->lightsCharacteristics type:CBCharacteristicWriteWithoutResponse];
}

-(void)setHeadLightsOn:(bool)value
{
	[self updateLights:value:0];
}

-(void)setLeftTurnLightsOn:(bool)value
{
	[self updateLights:value:1];
}

-(void)setRightTurnLightsOn:(bool)value
{
	[self updateLights:value:2];
}

-(void)setReverseLightsOn:(bool)value
{
	[self updateLights:value:3];
}

-(void)setBreakLightsOn:(bool)value
{
	[self updateLights:value:4];
}

-(bool)headLightsOn
{
	return ((self->lightsState & 1) > 0);
}

-(bool)leftTurnLightsOn
{
	return ((self->lightsState & 2) > 0);
}

-(bool)rightTurnLightsOn
{
	return ((self->lightsState & 3) > 0);
}

-(bool)reverseLightsOn
{
	return ((self->lightsState & 4) > 0);
}

-(bool)breakLightsOn
{
	return ((self->lightsState & 5) > 0);
}

-(void)updateValue:(CBCharacteristic*)characteristics
{
    if (nil != characteristics)
    {
        [self->device readValueForCharacteristic:characteristics];
    }
}

-(void)updateValues
{
    [self updateValue:self->temperatureCharacteristics];
    [self updateValue:self->batteryVoltageCharacteristics];
    [self updateValue:self->controllerBatteryVoltageCharacteristics];
    [self updateValue:self->batteryCurrentCharacteristics];
    [self updateValue:self->accelerometerValueCharacteristics];
}

-(void)loadValues
{
    [self updateValue:self->batteryMaxVoltageCharacteristics];
    [self updateValue:self->batteryCapacityCharacteristics];
    [self updateValue:self->nameCharacteristics];
    
    [self updateValue:self->minSteeringCharacteristics];
    [self updateValue:self->maxSteeringCharacteristics];
    [self updateValue:self->centerSteeringCharacteristics];
    
    [self updateValue:self->minThrottleCharacteristics];
    [self updateValue:self->maxThrottleCharacteristics];
    [self updateValue:self->centerThrottleCharacteristics];
}

-(CBUUID*)toCBUUID:(int)value
{
    int serviceUUID = value;

    UInt16 c = [Utils swap:serviceUUID];

    NSData* cd = [[NSData alloc] initWithBytes:(char*)&c length:2];

    CBUUID* su = [CBUUID UUIDWithData:cd];

    return su;
}

-(void)startServicesSeach
{
    CBUUID* su1 = [self toCBUUID:CAR_SERVICE_UUID];
    CBUUID* su2 = [self toCBUUID:TEMPERATURE_SERVICE_UUID];
    CBUUID* su3 = [self toCBUUID:BATTERY_SERVICE_UUID];
    CBUUID* su4 = [self toCBUUID:ACCELEROMETER_SERVICE_UUID];
    
    NSArray* services = [NSArray arrayWithObjects:su1, su2, su3, su4, nil];
    
    [self->device discoverServices:services];
}

-(id)initWithData:(CBPeripheral*)peripheral :(id<CarDelegate>)delegate;
{
    self = [super init];
    
    if (self)
    {
        self.delegateConnect = delegate;
        self->device = peripheral;
        self->device.delegate = self;

        [self startServicesSeach];
        
        self->steeringMinValue = 200;
        self->steeringMaxValue = 550;
        self->steeringCenterValue = 375;
        
        self->throttleMinValue = 200;
        self->throttleMaxValue = 550;
        self->throttleCenterValue = 375;
        
        self->nameValue = self->device.name;
        
        self->timer = [NSTimer scheduledTimerWithTimeInterval:1 target:self selector:@selector(updateValues) userInfo:nil repeats:YES];
        
        [timer fire];
    }
    
    return self;
}

-(void)disconnect
{
    [self->timer invalidate];

    [self enableAccelerometer:false];
    [[CarsController getInstance] disconnectActive:self->device];
}

-(void)setThrottle:(int)value
{
    if (value != self->throttleValue)
    {
        [self setCharacteristics:self->throttleCharacteristics :value];
        self->throttleValue = value;
    }
}

-(int)getThrottle
{
    return self->throttleValue;
}

-(void)setSteering:(int)value
{
    if (fabs(self->steeringValue - value) > 1)
    {
        [self setCharacteristics:self->steeringCharacteristics:value];
        self->steeringValue = value;
    }
}

-(int)getSteering
{
    return self->steeringValue;
}

-(void)peripheralDidUpdateName:(CBPeripheral*)peripheral
{
}

-(void)peripheralDidInvalidateServices:(CBPeripheral*)peripheral
{
}

-(void)peripheralDidUpdateRSSI:(CBPeripheral*)peripheral error:(NSError*)error
{
    NSLog(@"RSSI: %d", (int)peripheral.RSSI);
}

-(void)peripheral:(CBPeripheral*)peripheral didDiscoverServices:(NSError*)error
{
    if (!error)
    {
        NSLog(@"Services of peripheral with UUID : %@ found", peripheral.identifier.UUIDString);

        for (int i = 0; i < peripheral.services.count; i++)
        {
            CBService* service = [peripheral.services objectAtIndex:i];
            
            NSLog(@"Fetching characteristics for service with UUID : %@", service.UUID);
            
            [peripheral discoverCharacteristics:nil forService:service];
        }
    }
    else
    {
        NSLog(@"Service discovery was unsuccessfull!");
    }
}

-(void)peripheral:(CBPeripheral*)peripheral didDiscoverIncludedServicesForService:(CBService*)service error:(NSError*)error
{
}

-(void)peripheral:(CBPeripheral*)peripheral didDiscoverCharacteristicsForService:(CBService*)service error:(NSError*)error
{
    if (!error)
    {
        NSLog(@"Characteristics of service with UUID : %@ found", service.UUID);
        
        for (int i = 0; i < service.characteristics.count; i++)
        {
            CBCharacteristic* characteristic = [service.characteristics objectAtIndex:i];
            
            NSLog(@"Found characteristic %@", characteristic.UUID);
            
            UInt16 uuid = [Utils CBUUIDToInt:characteristic.UUID];
            
            switch (uuid)
            {
                case 65473:
                    self->throttleCharacteristics = characteristic;
                    break;
                case 65474:
                    self->steeringCharacteristics = characteristic;
                    break;
                case 41218:
                    self->temperatureCharacteristics = characteristic;
                    break;
                case 0xe101:
                    self->batteryVoltageCharacteristics = characteristic;
                    break;
                case 10777:
                    self->controllerBatteryVoltageCharacteristics = characteristic;
                    break;
                case 0xf101:
                    self->nameCharacteristics = characteristic;
                    break;
                case 0xaa01:
                    self->temperatureCharacteristics = characteristic;
                    break;
                case 0xe103:
                    self->batteryMaxVoltageCharacteristics = characteristic;
                    break;
                case 0xe102:
                    self->batteryCurrentCharacteristics = characteristic;
                    break;
                case 0xb101:
                    self->minThrottleCharacteristics = characteristic;
                    break;
                case 0xb102:
                    self->maxThrottleCharacteristics = characteristic;
                    break;
                case 0xb103:
                    self->centerThrottleCharacteristics = characteristic;
                    break;
                case 0xc101:
                    self->minSteeringCharacteristics = characteristic;
                    break;
                case 0xc102:
                    self->maxSteeringCharacteristics = characteristic;
                    break;
                case 0xc103:
                    self->centerSteeringCharacteristics = characteristic;
                    break;
                case 0xe104:
                    self->batteryCapacityCharacteristics = characteristic;
                    break;
                case 0xd101:
                    self->pulseWidthCharacteristics = characteristic;
                    break;
                case 0xa101:
                    self->lightsCharacteristics = characteristic;
                    break;
                case 0xe105: // battery_used
                case 0xffb2: // ligts sensor
                    break;
                case 0xaa11:
                    self->accelerometerValueCharacteristics = characteristic;
                    break;
                case 0xaa12:
                    self->accelerometerEnableCharacteristics = characteristic;
                    [self enableAccelerometer:true];
                    break;
                case 0xaa13: // accelerometer update period
                    break;
                default:
                    NSLog(@"Unhandled characteristic: %f", (float)uuid);
                    break;
            }
        }
        
        if (self->throttleCharacteristics && self->steeringCharacteristics)
        {
            NSLog(@"Finished discovering characteristics");
            
            [self loadValues];
            
            [self.delegateConnect connected];
        }
    }
    else
    {
        NSLog(@"Characteristic discorvery unsuccessfull!");
    }
}

-(uint16_t)getValue:(CBCharacteristic*)characteristic
{
    uint16_t temp = 0;
    
    [characteristic.value getBytes:&temp length:sizeof(temp)];
    
    return temp;
}

-(float)getAccelerometerValue:(int8_t)v
{
    return (float)v / 64.0f;
}

-(void)readAccelerometerValue:(CBCharacteristic*)characteristic
{
    int8_t temp[3] = { 0 };
    
    [characteristic.value getBytes:&temp length:sizeof(int8_t) * 3];
    
    self.accelerationX = [self getAccelerometerValue:temp[0]];
    self.accelerationY = [self getAccelerometerValue:temp[1]];
    self.accelerationZ = [self getAccelerometerValue:temp[2]];
}

-(void)peripheral:(CBPeripheral*)peripheral didUpdateValueForCharacteristic:(CBCharacteristic*)characteristic error:(NSError*)error
{
    if (error)
    {
        return;
    }
    
    if (characteristic == self->temperatureCharacteristics)
    {
        self.temperature = (float)[self getValue:characteristic] / 100.0f;
    }
    else if (characteristic == self->batteryVoltageCharacteristics)
    {
        self.mainBatteryVoltage = (float)[self getValue:characteristic] / 10.0f;
    }
    else if (characteristic == self->controllerBatteryVoltageCharacteristics)
    {
        self.controllerBatteryLevel = (float)[self getValue:characteristic];
    }
    else if (characteristic == self->batteryMaxVoltageCharacteristics)
    {
        self->maxBatteryVoltageValue = (float)[self getValue:characteristic] / 10.0f;
    }
    else if (characteristic == self->batteryCurrentCharacteristics)
    {
        self.loadCurrent = (float)[self getValue:characteristic] / 10000.0f;
    }
    else if (characteristic == self->batteryCapacityCharacteristics)
    {
        self->maxBatteryCapacityValue = [self getValue:characteristic];
    }
    else if (characteristic == self->nameCharacteristics)
    {
        self->nameValue = [[NSString alloc] initWithData:characteristic.value encoding:NSUTF8StringEncoding];
    }
    else if (characteristic == self->minSteeringCharacteristics)
    {
        self->steeringMinValue = [self getValue:characteristic];
    }
    else if (characteristic == self->maxSteeringCharacteristics)
    {
        self->steeringMaxValue = [self getValue:characteristic];
    }
    else if (characteristic == self->centerSteeringCharacteristics)
    {
        self->steeringCenterValue = [self getValue:characteristic];
    }
    else if (characteristic == self->minThrottleCharacteristics)
    {
        self->throttleMinValue = [self getValue:characteristic];
    }
    else if (characteristic == self->maxThrottleCharacteristics)
    {
        self->throttleMaxValue = [self getValue:characteristic];
    }
    else if (characteristic == self->centerThrottleCharacteristics)
    {
        self->throttleCenterValue = [self getValue:characteristic];
    }
    else if (characteristic == self->accelerometerValueCharacteristics)
    {
        [self readAccelerometerValue:characteristic];
    }
    else
    {
        NSLog(@"Unhandled characteristic value read: %@ with ID %@", characteristic.value, characteristic.UUID);
    }
}

-(void)peripheral:(CBPeripheral*)peripheral didWriteValueForCharacteristic:(CBCharacteristic*)characteristic error:(NSError*)error
{
    if (error)
    {
        NSLog(@"Error while witing characteristics");

        return;
    }
}

-(void)peripheral:(CBPeripheral*)peripheral didUpdateNotificationStateForCharacteristic:(CBCharacteristic*)characteristic error:(NSError*)error
{
}

-(void)peripheral:(CBPeripheral*)peripheral didDiscoverDescriptorsForCharacteristic:(CBCharacteristic*)characteristic error:(NSError*)error
{
}

-(void)peripheral:(CBPeripheral*)peripheral didUpdateValueForDescriptor:(CBDescriptor*)descriptor error:(NSError*)error
{
}

-(void)peripheral:(CBPeripheral*)peripheral didWriteValueForDescriptor:(CBDescriptor*)descriptor error:(NSError*)error
{
}

@end
