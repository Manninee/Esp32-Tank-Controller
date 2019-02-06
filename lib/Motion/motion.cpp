#include "motion.hh"

Motion::Motion(uint8_t deadzone): 
accelgyro_(new MPU6050), deadzone_(deadzone)
{}

void Motion::initAccelGyro(uint8_t sda, uint8_t scl)
{
    Wire.begin(sda, scl, 400000);
    accelgyro_->initialize();

    // Sets offsets for the MPU6050 module
    accelgyro_->setXAccelOffset(-506);
    accelgyro_->setYAccelOffset(-3396);
    accelgyro_->setZAccelOffset(1778);

    // Offsets for the gyro (not used)
    /*
    accelgyro_->setXGyroOffset (79);
    accelgyro_->setYGyroOffset (-3);
    accelgyro_->setZGyroOffset (5);
    */
}

Motion::angles Motion::getAngles()
{
    int16_t ax = 0, ay = 0, az = 0;
    accelgyro_->getAcceleration(&ax, &ay, &az);
    
    // Check if angle is below horizontal
    if( az < 0) az = -1;

    // Calculate angles for x and y
    double y = atan(ax / sqrt(ay * ay + az * az)) * RAD_TO_DEG;
    double x = atan(ay / sqrt(ax * ax + az * az)) * RAD_TO_DEG;

    // Check for the maximum angle of +-85 degrees
    if(y > 85) y = 85;
    else if(y < -85) y = -85;

    if(x > 85) x = 85;
    else if(x < -85) x = -85;

    // Add center offset to x
    if(x < deadzone_  && x > -deadzone_) 
        x = 0;
    else if(x > 0)
        x = x - deadzone_;
    else
        x = x + deadzone_;

    // Add center offset to y
    if(y < deadzone_  && y > -deadzone_) 
        y = 0;
    else if(y > 0) 
        y = y - deadzone_;
    else 
        y = y + deadzone_;

    angles pair;
    pair.y = y;
    pair.x = x;
    return pair;
}

Motion::angles Motion::getAvarageAngles()
{
    // Gets ten angle values and avarages them
    double y = 0, x = 0;
    for(uint8_t i = 0; i < 10; ++i)
    {
        Motion::angles a = getAngles();
        y += a.y;
        x += a.x;
    }

    angles pair;
    pair.y = y / 10;
    pair.x = x / 10;
    return pair;
}
