#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
#include <WiFiUDP.h>
#include <EEPROM.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer server(80);
Servo myservo;
Servo esc;
WiFiUDP Udp;
byte packetBuffer[512];
unsigned long lastPacket = millis();
ESP8266HTTPUpdateServer httpUpdater;
int light0Pin = 13;
int light1Pin = 12;
int light2Pin = 16;
int light3Pin = 14;
int light4Pin = 2;
int escPin = 4;
int servoPin = 5;

void handleNotFound()
{
  String message = "File Not Found\n\n";

  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
};

void handleSettingsValue(uint8_t address)
{
  String value = server.arg("value");

  if (value.length() > 0)
  {
    uint16_t intValue = value.toInt();

    EEPROM.write(address + 0, intValue >> 8);
    EEPROM.write(address + 1, (uint8_t)intValue);

    EEPROM.commit();

    server.send(200, "text/plain", String(intValue));
  }
  else
  {
    uint16_t intValue = (EEPROM.read(address + 0) << 8) | (EEPROM.read(address + 1));
    server.send(200, "text/plain", String(intValue));
  }
};

String getName()
{
  String name = "";

  for (int i = 0; i < 20; i++)
  {
    uint8_t intValue = EEPROM.read(12 + i);

    if ((intValue == 0x00) || (intValue == 0xff))
    {
      break;
    }
    else if (((intValue >= 'a') && (intValue <= 'z')) || ((intValue >= 'A') && (intValue <= 'Z')) || ((intValue >= '0') && (intValue <= '9')) || (intValue == '-') || (intValue == ' ') || (intValue == '_'))
    {
      name += (char)intValue;
    }
  }

  if (name.length() <= 0)
  {
    name = String(ESP.getChipId());
  }

  return name;
};

void handleNameValue()
{
  String value = server.arg("value");

  if (value.length() > 0)
  {
    int i = 0;

    for (i = 0; (i < 19) && (i < value.length()); i++)
    {
      uint8_t v = (uint8_t)value.c_str()[i];

      EEPROM.write(12 + i, v);
    }

    EEPROM.write(12 + i, 0);

    EEPROM.commit();

    server.send(200, "text/plain", value + "->" + getName());
  }
  else
  {
    server.send(200, "text/plain", getName());
  }
};

void handleLight(int pin, bool isOn)
{
  pinMode(pin, OUTPUT);    
  
  if (isOn)
  {
    digitalWrite(pin, LOW);
    server.send(200, "text/plain", "on");
  }
  else
  {
    digitalWrite(pin, HIGH);
    server.send(200, "text/plain", "off");
  }
};

void registerHandlers()
{
  server.on("/settings/steering/min", []() { handleSettingsValue(0); });
  server.on("/settings/steering/max", []() { handleSettingsValue(2); });
  server.on("/settings/steering/center", []() { handleSettingsValue(4); });
  server.on("/settings/throttle/min", []() { handleSettingsValue(6); });
  server.on("/settings/throttle/max", []() { handleSettingsValue(8); });
  server.on("/settings/throttle/center", []() { handleSettingsValue(10); });
  server.on("/settings/name", []() { handleNameValue(); });
  server.on("/", []() { server.send(200, "text/plain", "hello from " + getName()); });
  server.on("/lights/0/on", []() { handleLight(light0Pin, true); });
  server.on("/lights/0/off", []() { handleLight(light0Pin, false); });
  server.on("/lights/1/on", []() { handleLight(light1Pin, true); });
  server.on("/lights/1/off", []() { handleLight(light1Pin, false); });
  server.on("/lights/2/on", []() { handleLight(light2Pin, true); });
  server.on("/lights/2/off", []() { handleLight(light2Pin, false); });
  server.on("/lights/3/on", []() { handleLight(light3Pin, true); });
  server.on("/lights/3/off", []() { handleLight(light3Pin, false); });
  server.on("/lights/4/on", []() { handleLight(light4Pin, true); });
  server.on("/lights/4/off", []() { handleLight(light4Pin, false); });
  server.onNotFound(handleNotFound);
};

void registerFirmwareUpdater()
{
  httpUpdater.setup(&server);
};

void setup(void)
{
  Serial.begin(115200);

  EEPROM.begin(512);

  String apName = "MyRC-" + getName();
  String apPass = "magicpass123";

  Serial.println("Starting ap [" + apName + "] with [" + apPass + "]");

  if (false == WiFi.softAP(apName.c_str(), apPass.c_str()))
  {
    Serial.println("WiFi start failed.. Resetting settings");

    ESP.eraseConfig();
    ESP.reset();
  }
  else
  {
    Serial.println("WiFi start ok");
  }

  server.on("/state", []()
  {
    String servo = server.arg("servo");
    int servoPosition = servo.toInt();

    String escArg = server.arg("esc");
    int escPosition = escArg.toInt();

    setServo(servoPosition);
    setEsc(escPosition);

    server.send(200, "text/plain", "this works as well");
  });

  server.on("/battery", []()
  {
    server.send(200, "text/plain", String(analogRead(A0)));
  });

  registerHandlers();
  registerFirmwareUpdater();

  server.begin();
  Serial.println("HTTP server started");

  Udp.begin(9999);

  handleLight(light0Pin, false);
  handleLight(light1Pin, false);
  handleLight(light2Pin, false);
  handleLight(light3Pin, false);
  handleLight(light4Pin, false);

  pinMode(servoPin, INPUT);
  pinMode(escPin, INPUT);
}

void checkConnectionTimeOut()
{
  unsigned long elapsedTime =  millis() - lastPacket;

  if (elapsedTime > 1000)
  {
    setServo(0xffff);
    setEsc(0xffff);
  }  
};

void setServo(int value)
{
  if (value != 0xffff)
  {
    if (false == myservo.attached())
    {
      pinMode(servoPin, OUTPUT);      
      myservo.attach(servoPin);
    }
    
    myservo.writeMicroseconds(value);
  }
  else
  {
    myservo.detach();
    pinMode(servoPin, INPUT);
  }
};

void setEsc(int value)
{
  if (value != 0xffff)
  {
    if (false == esc.attached())
    {
      pinMode(escPin, OUTPUT);
      esc.attach(escPin);
    }
    
    esc.writeMicroseconds(value);
  }
  else
  {
    esc.detach();
    pinMode(escPin, INPUT);
  }
};

void handleUdpCommands()
{
  int noBytes = Udp.parsePacket();
  String received_command = "";

  if (noBytes)
  {
    lastPacket = millis();

    Udp.read(packetBuffer, noBytes);

    int st = (packetBuffer[0] << 8) | packetBuffer[1];
    int th = (packetBuffer[2] << 8) | packetBuffer[3];

    Serial.println(st, DEC);
    Serial.println(th, DEC);

    setServo(st);
    setEsc(th);
  }
};

void loop(void)
{
  server.handleClient();

  handleUdpCommands();

  checkConnectionTimeOut();
};

