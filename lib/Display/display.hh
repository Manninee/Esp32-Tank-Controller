#ifndef DISPLAY_HH
#define DISPLAY_HH

#include <Arduino.h>
#include "SSD1306Wire.h"

class Display
{
    public:
        Display(uint8_t sda, uint8_t scl);
        void init();
        void drawJoystickValues(const double& roll, const double& pitch);
        void drawLatency(const int16_t latency);
        void drawBattery(const int16_t voltage);

    private:
        void clearPart(const uint8_t& x0, const uint8_t& y0,
                    const uint8_t& width, const uint8_t& height);

        SSD1306Wire* display_;
};

#endif