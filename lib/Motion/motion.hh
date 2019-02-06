#ifndef MOTION_hh
#define MOTION_hh

#include <Arduino.h>
#include "MPU6050.h"

class Motion
{
    public:
        struct angles
        {
            double y;
            double x;
        };

        Motion(uint8_t deadzone);
        void initAccelGyro(uint8_t sda, uint8_t scl);
        angles getAngles();
        angles getAvarageAngles();

    private:
        MPU6050* accelgyro_;
        uint8_t deadzone_;

};


#endif