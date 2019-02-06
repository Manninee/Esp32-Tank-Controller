#include <Arduino.h>
#include <WiFi.h>
#include <Ticker.h>

#include "motion.hh"
#include "network.hh"
#include "display.hh"

#define SDA 5
#define SCL 4
#define LED 16
#define PORT 8080
#define REMOTEPORT 8080

const char * SSID = "Tank";
const char * PWD = "Something";

IPAddress IP(192, 168, 1, 230);
IPAddress GATEWAY(192, 168, 1, 250);
IPAddress SUBNET(255, 255, 255, 0);

#define DEADZONE 5
Motion motion(DEADZONE);
Display display(SDA, SCL);
Network network;

Ticker motionTimer;
Ticker latencyTimer;
Ticker secondTimer;

volatile bool readAngle = false;
volatile bool getLatency = false;
volatile bool secondPassed = false;

void handleMotion();

void setup()
{
  network.initNetwork(IP, GATEWAY, SUBNET, SSID, PWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  motion.initAccelGyro(SDA, SCL);
  
  display.init();
  display.drawLatency(-1);
  display.drawBattery(-1);
  display.drawJoystickValues(0,0);

  network.initReceiver(PORT);
  network.initTransmitter(GATEWAY, REMOTEPORT, PORT);

  motionTimer.attach_ms(25, [](){readAngle = true;});
  latencyTimer.attach_ms(200, [](){getLatency = true;});
  secondTimer.attach(1, [](){secondPassed = true;});

  pinMode(LED, OUTPUT);
}

void loop() 
{
  bool packetReceived = network.packetHandler();

  if(packetReceived)
  {
    switch (network.getPacketType())
    {
      case Network::LATENCY:
        display.drawLatency(network.getLatency());
        break;

      case Network::BATTERY:
        display.drawBattery(network.getBatteryVoltage());
        break;
    
      default:
        break;
    }
  }

  // Check and send angle values every 25ms
  if(readAngle)
  {
    handleMotion();
    readAngle = false;
  }
  
  // Check latency every 200ms
  if(getLatency)
  {
    network.sendLatencyPacket();
    getLatency = false;
  }

  if(secondPassed)
  {
    if(WiFi.status() == WL_CONNECTED)
      digitalWrite(LED, HIGH);
    else
      digitalWrite(LED, LOW);

    if(!network.connected())
    {
      display.drawJoystickValues(0,0);
      display.drawLatency(-1);
      display.drawBattery(-1);
    }
    secondPassed = false;
  }
}

void handleMotion()
{
  Motion::angles a = motion.getAvarageAngles();
  display.drawJoystickValues(a.x, a.y);
  network.sendAngles(a.x, a.y);
}