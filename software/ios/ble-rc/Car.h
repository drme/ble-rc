#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import <CoreBluetooth/CBService.h>

#define CAR_SERVICE_UUID            0xACC0
#define BATTERY_SERVICE_UUID        0x180F
#define TEMPERATURE_SERVICE_UUID    0xAA00
#define ACCELEROMETER_SERVICE_UUID  0xAA10

@protocol CarDelegate

-(void)connected;
-(void)disconnected;

@end

@interface Car : NSObject<CBPeripheralDelegate>

@property (nonatomic,assign) id<CarDelegate> delegateConnect;
@property (assign) float accelerationX;
@property (assign) float accelerationY;
@property (assign) float accelerationZ;
@property (assign) float temperature;






@property (assign) float controllerBatteryLevel;

@property (assign) bool invertThrottle;
@property (assign) bool headLightsOn;
@property (assign) bool leftTurnLightsOn;
@property (assign) bool rightTurnLightsOn;
@property (assign) bool reverseLightsOn;
@property (assign) bool breakLightsOn;

@property (assign) float chipTemperature;


@property (assign) float mainBatteryVoltage;
@property (assign) float loadCurrent;

@property (assign) float mainBatteryUsed;
@property (assign) bool mainBatteryCharging;
@property (assign) bool mainBatteryCharged;





@property (getter=getName, setter=setName:) NSString* name;
@property (getter=getMainBatteryMaxVoltage, setter=setMainBatteryMaxVoltage:) int mainBatteryMaxVoltage;
@property (getter=getMainBatteryCapacity, setter=setMainBatteryCapacity:) int mainBatteryCapacity;
@property (getter=getSteeringMin, setter=setSteeringMin:) int steeringMin;
@property (getter=getSteeringMax, setter=setSteeringMax:) int steeringMax;
@property (getter=getSteeringCenter, setter=setSteeringCenter:) int steeringCenter;
@property (getter=getThrottleMin, setter=setThrottleMin:) int throttleMin;
@property (getter=getThrottleMax, setter=setThrottleMax:) int throttleMax;
@property (getter=getThrottleCenter, setter=setThrottleCenter:) int throttleCenter;
@property (getter=getSteering, setter=setSteering:) int steering;
@property (getter=getThrottle, setter=setThrottle:) int throttle;

-(id)initWithData:(CBPeripheral*)peripheral :(id<CarDelegate>)delegate;
-(void)disconnect;

@end
