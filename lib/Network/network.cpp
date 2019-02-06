#include "network.hh"

//#define DEBUG

Network::Network():
udp_(new WiFiUDP),
remoteIpAddress_(IPAddress(0,0,0,0)),
remotePort_(0),
connected_(false),
latencyCounter_(0),
timeCounter_(0),
packetCounter_(1),
packetType_(NONE),
latency_(0),
batteryVoltage_(0)
{}

void Network::initNetwork(IPAddress& ip, IPAddress& gateway, IPAddress& subnet,
                        const char* ssid, const char* password)
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.config(ip, gateway, subnet);
    auto f = std::bind(&Network::wifiEventHandler, this, std::placeholders::_1);
    WiFi.onEvent(f);
    WiFi.begin(ssid, password);
}

void Network::initReceiver(const uint16_t& port)
{
   udp_->begin(port);
}

void Network::initTransmitter(IPAddress& tankIp, const uint16_t& remotePort, const uint16_t& port)
{
    remoteIpAddress_ = tankIp;
    remotePort_ = remotePort;
    packetCounter_ = 1;
    connected_ = true;
    sendStartPacket(port);
}

void Network::closeTransmitter()
{
    sendChar('D');
    connected_ = false;
    remoteIpAddress_ = IPAddress(0,0,0,0);
    remotePort_ = 0;
    packetCounter_ = 1;
    latencyCounter_ = 0;
    timeCounter_ = 0;
}

void Network::sendAngles(const double& x, const double& y)
{
    if(!connected_) return;

    // Map angle range (-80)-(+80) to values (-1023)-(+1023)
    int16_t ix = round(x * 10);
    ix = map(ix, -800, 800, -1023, 1023);
    int16_t iy = round(y * 10);
    iy = map(iy, -800 , 800, -1023, 1023);

    // Add offset to values for sending (Removed in the tank receiver)
    ix += 1023;
    iy += 1023;

    uint8_t buffer[5] = {'A', (uint8_t)(ix >> 8), (uint8_t)ix, 
                        (uint8_t)(iy >> 8), (uint8_t)iy};
    sendBufferWithCounter(buffer, 5);
}

void Network::sendLatencyPacket()
{
    if(!connected_) return;

    uint8_t buffer[1] = {'P'};
    sendBufferWithCounter(buffer, 1);

    if(timeCounter_ == 0)
        timeCounter_ = esp_timer_get_time();
    
    ++latencyCounter_;
    if(latencyCounter_ > 2)
        closeTransmitter();
}

uint16_t Network::getLatency()
{
    packetType_ = NONE;
    return latency_;
}

uint16_t Network::getBatteryVoltage()
{
    packetType_ = NONE;
    return batteryVoltage_;
}

bool Network::connected()
{
    return connected_;
}

Network::PACKET_TYPE Network::getPacketType()
{
    return packetType_;
}

bool Network::packetHandler()
{
    uint8_t packetSize = udp_->parsePacket();
    if(!packetSize)
        return 0;
    
    if(udp_->remoteIP() != remoteIpAddress_)
        return 0;

    latencyCounter_ = 0;
    packetType_ = NONE;

    uint8_t data[packetSize];
    udp_->read(data, packetSize);

    switch (data[0])
    {
    case LATENCY:
    {
        uint64_t currentTime = esp_timer_get_time();
        if(currentTime > timeCounter_)
        {
            latency_ = (currentTime - timeCounter_) / 1000;
            packetType_ = LATENCY;
        }
        
        timeCounter_ = 0;
        break;
    }
    case BATTERY:
    {
        if(packetSize < 3)
            return 0;

        batteryVoltage_ = (uint16_t)(data[1] << 8);
        batteryVoltage_ |= (uint16_t)data[2];
        packetType_ = BATTERY;
        break;
    }
    
    default:
        break;
    }
    return 1;
}

void Network::wifiEventHandler(WiFiEvent_t event)
{
    switch (event)
    {
        case SYSTEM_EVENT_STA_CONNECTED:
            break;
    
        case SYSTEM_EVENT_STA_DISCONNECTED:
            closeTransmitter();
            break;
        default:
            break;
    }
}

void Network::sendStartPacket(const uint16_t& port)
{
    // Send start packet
    uint8_t buffer[3] = {'S', 0, 0};
    buffer[1] = (uint8_t)(port >> 8);
    buffer[2] = (uint8_t)port;

    sendBuffer(buffer, 3);
}

void Network::sendChar(const char character)
{
    udp_->beginPacket(remoteIpAddress_, remotePort_);
    udp_->write(character);
    udp_->endPacket();
}

void Network::sendString(String message)
{
    if(!connected_) return;

    message = "S" + message;
    uint8_t size = message.length() + 1;
    uint8_t buffer[size];
    message.getBytes(buffer, size);
    sendBufferWithCounter(buffer, size);
}

void Network::sendBufferWithCounter(uint8_t *buffer, uint8_t len)
{
    // Add packet to counter and check for overflow
    ++packetCounter_;
    if(packetCounter_ == 0) sendResetCounter();
    
    uint8_t newLen = len + 5;
    uint8_t newBuffer[newLen];

    // Add counter values to packet
    newBuffer[0] = 'C';
    for(uint8_t i = 0; i < 4; ++i)
    {
        uint8_t shiftCount = 8 *(3 - i);
        newBuffer[i + 1] = (uint8_t)(packetCounter_ >> shiftCount);
    }

    // Add message to packet
    for(uint8_t i = 5; i < newLen; ++i)
        newBuffer[i] = buffer[i - 5];
    
    sendBuffer(newBuffer, newLen);
}

void Network::sendBuffer(uint8_t *buffer, uint8_t len)
{
    udp_->beginPacket(remoteIpAddress_, remotePort_);
    udp_->write(buffer, len);
    udp_->endPacket();
}

void Network::sendResetCounter()
{
    packetCounter_ = 1;
    sendChar('R');
}