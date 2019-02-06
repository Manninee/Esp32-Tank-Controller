# Esp32 Tank Controller

Espressif 32 wireless motion controller for a robot tank. Controller controls the tank over wifi using simple udp packets.

## Getting Started

Parts list:
- Espressif 32 microcontroller.
- MPU6050 Accelerometer and Gyro.
- SSD1306 based oled screen.
- Power supply with the right voltages.

Project is writen in PlatformIO version 1.30.2.  
Project uses 3 external libraries which are found in the PlatformIO library manager.  
The 3 libraries are:
- I2Cdevlib-Core
- I2Cdevlib-MPU6050
- ESP8266 and ESP32 Oled Driver for SSD1306 display

Project also uses libraries that come with the PlatformIO Arduino core for the Esp32.
