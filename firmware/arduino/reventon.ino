/**
 * Bluetooth control for toy car motors.
 * Accepts commands over bluetooth and drives motors according them.
 * schematics: rc-reventon-arduino.fzz 
 */
#include <IRremote.h>
#include <SoftwareSerial.h>

#define MAX_MILLIS_TO_WAIT 100

const int serialRX = 4;
const int serialTX = 2;

SoftwareSerial mySerial(serialRX, serialTX);
IRsend irsend;

const int None = 0;
const int Left = 1;
const int Right = 2;
const int Back = 4;
const int Forward = 8;
const int Horn = 16;
const int HandBrake = 32;
const int Off = 64;

const int motorFrontWheels1APin = 8;    // H-bridge leg 1 (pin 2, 1A)
const int motorFrontWheels2APin = 12;   // H-bridge leg 2 (pin 7, 2A)
const int enableFrontWheels12Pin = 7;   // H-bridge enable pin
const int enableRearWheels34Pin = 10;   // H-bridge enable pin
const int motorRearWheels4APin = 9;     // H-bridge leg 1 (pin 2, 4A)
const int motorRearWheels3APin = 11;    // H-bridge leg 2 (pin 7, 2A)

const int frontLeftTurnPin = A2;
const int frontRightTurnPin = A0;
const int rearLeftTurnPin = A3;
const int rearRightTurnPin = 13;
const int backPin = A1;

const int hornPin = 5;

void setup()
{
  pinMode(motorFrontWheels1APin, OUTPUT);
  pinMode(motorFrontWheels2APin, OUTPUT);
  pinMode(enableFrontWheels12Pin, OUTPUT);

  pinMode(motorRearWheels4APin, OUTPUT);
  pinMode(motorRearWheels3APin, OUTPUT);
  pinMode(enableRearWheels34Pin, OUTPUT);

  powerOffMotors();

  pinMode(serialRX, INPUT);
  pinMode(serialTX, OUTPUT);

  pinMode(backPin, OUTPUT);
  pinMode(frontLeftTurnPin, OUTPUT);
  pinMode(frontRightTurnPin, OUTPUT);
  pinMode(rearLeftTurnPin, OUTPUT);
  pinMode(rearRightTurnPin, OUTPUT);

  pinMode(hornPin, OUTPUT);
  
  beep(50);
  beep(50);
  beep(50); 

  mySerial.begin(9600);
}
  
void loop()
{
  unsigned long starttime = millis();

  while ((mySerial.available()<1) && ((millis() - starttime) < MAX_MILLIS_TO_WAIT))
  {
  }

  if (mySerial.available() <= 0)
  {
    powerOffMotors();
  }
  else
  {
    int code = mySerial.read();
   
    handleCar(code);
  }
}

void handleCar(int incomingByte)
{
  if ((incomingByte & Left) > 0)
  {
    digitalWrite(enableFrontWheels12Pin, HIGH);      
    digitalWrite(motorFrontWheels1APin, HIGH);
    digitalWrite(motorFrontWheels2APin, LOW);

    digitalWrite(frontLeftTurnPin, HIGH);
    digitalWrite(frontRightTurnPin, LOW);
    digitalWrite(rearLeftTurnPin, HIGH);
    digitalWrite(rearRightTurnPin, LOW);
  }
  else if ((incomingByte & Right) > 0)
  {
    digitalWrite(enableFrontWheels12Pin, HIGH);
    digitalWrite(motorFrontWheels1APin, LOW);
    digitalWrite(motorFrontWheels2APin, HIGH);

    digitalWrite(frontLeftTurnPin, LOW);
    digitalWrite(frontRightTurnPin, HIGH);
    digitalWrite(rearLeftTurnPin, LOW);
    digitalWrite(rearRightTurnPin, HIGH);
  }
  else
  {
    digitalWrite(enableFrontWheels12Pin, LOW);
    digitalWrite(motorFrontWheels1APin, LOW);
    digitalWrite(motorFrontWheels2APin, LOW);

    digitalWrite(frontLeftTurnPin, LOW);
    digitalWrite(frontRightTurnPin, LOW);
    digitalWrite(rearLeftTurnPin, LOW);
    digitalWrite(rearRightTurnPin, LOW);
  }
  
  if ((incomingByte & Back) > 0)
  {
    digitalWrite(enableRearWheels34Pin, HIGH);
    digitalWrite(motorRearWheels4APin, LOW);
    digitalWrite(motorRearWheels3APin, HIGH);
    
    digitalWrite(rearLeftTurnPin, HIGH);
    digitalWrite(rearRightTurnPin, HIGH);
    digitalWrite(backPin, HIGH);
  }
  else if ((incomingByte & Forward) > 0)
  {
    digitalWrite(enableRearWheels34Pin, HIGH);
    digitalWrite(motorRearWheels4APin, HIGH);
    digitalWrite(motorRearWheels3APin, LOW);

    digitalWrite(backPin, LOW);
  }
  else
  {
    digitalWrite(enableRearWheels34Pin, LOW);
    digitalWrite(motorRearWheels4APin, LOW);
    digitalWrite(motorRearWheels3APin, LOW);
  }
  
  if ((incomingByte & Horn) > 0)
  {
    beep(50);
  }
  
  if ((incomingByte & Off) > 0)
  {
    beep(5);
    sendCode(0xa90);
  } 
}

void sendCode(int code)
{
  for (int i = 0; i < 3; i++)
  {
    irsend.sendSony(code, 12);
    delay(50);
  }
} 

void beep(unsigned char delayms)
{
  analogWrite(hornPin, 20);
  delay(delayms);
  analogWrite(hornPin, 0);
  delay(delayms);
}  

void powerOffMotors()
{
  digitalWrite(motorFrontWheels1APin, LOW);
  digitalWrite(motorFrontWheels2APin, LOW);
  digitalWrite(enableFrontWheels12Pin, LOW);
  digitalWrite(enableRearWheels34Pin, LOW);
  digitalWrite(motorRearWheels4APin, LOW);
  digitalWrite(motorRearWheels3APin, LOW);
  
  digitalWrite(frontLeftTurnPin, LOW);
  digitalWrite(frontRightTurnPin, LOW);  
  digitalWrite(rearLeftTurnPin, LOW);
  digitalWrite(rearRightTurnPin, LOW);  
  digitalWrite(backPin, LOW);
}

