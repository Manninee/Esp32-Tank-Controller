#ifndef NETWORK_hh
#define NETWORK_hh

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class Network
{
    public:
        enum PACKET_TYPE: char{NONE = ' ', LATENCY = 'P', BATTERY = 'B'};

        Network();
        void initNetwork(IPAddress& ip, IPAddress& gateway, IPAddress& subnet,
                        const char* ssid, const char* password);

        void initReceiver(const uint16_t& port);
        
        void initTransmitter(IPAddress& tankIp, const uint16_t& remotePort, const uint16_t& port);
        void closeTransmitter();
        void sendAngles(const double& x, const double& y);
        void sendLatencyPacket();

        uint16_t getLatency();
        uint16_t getBatteryVoltage();
        bool connected();
        bool packetHandler();
        PACKET_TYPE getPacketType();

    private:
        void wifiEventHandler(WiFiEvent_t event);
        void sendStartPacket(const uint16_t& port);
        void sendChar(const char character);
        void sendString(String message);
        void sendBufferWithCounter(uint8_t *buffer, uint8_t len);
        void sendBuffer(uint8_t *buffer, uint8_t len);
        void sendResetCounter();

        WiFiUDP* udp_;
        IPAddress remoteIpAddress_;
        uint16_t remotePort_;
        bool connected_;
        uint8_t latencyCounter_;
        uint64_t timeCounter_;
        uint32_t packetCounter_;
        PACKET_TYPE packetType_;
        uint16_t latency_;
        uint16_t batteryVoltage_;

};
#endif