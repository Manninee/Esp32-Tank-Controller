#include "display.hh"

Display::Display(uint8_t sda, uint8_t scl):
display_(new SSD1306Wire(0x3c, sda, scl))
{}

void Display::init()
{
    display_->init();
    display_->setContrast(255);
    display_->setFont(ArialMT_Plain_16);
    //display_->setFont(ArialMT_Plain_10);
    display_->clear();
    display_->display();
}

void Display::drawJoystickValues(const double& roll, const double& pitch)
{
    clearPart(0, 0, 128, 16);
    display_->drawString(0, 0, "X=" + String(roll));
    display_->drawString(64, 0, "Y=" + String(pitch));
    display_->display();
}

void Display::drawLatency(const int16_t latency)
{
    String text = "";

    if(latency < 0)
        text = "-----";
    else if(latency > 1000)
        text = ">1s";
    else
        text = String(latency) + "ms";

    clearPart(0, 47, 64, 16);
    display_->drawString(0, 47, text);
    display_->display();
}

void Display::drawBattery(const int16_t voltage)
{
    clearPart(64, 47, 64, 16);

    String text = "";
    if(voltage > 0)
        text = String((double)((double)voltage / 100)) + "V";
    else
        text = "----V";
    
    display_->drawString(64, 47, text);
    display_->display();
}

void Display::clearPart(const uint8_t& x0, const uint8_t& y0,
                        const uint8_t& width, const uint8_t& height)
{
    display_->setColor(BLACK);
    display_->fillRect(x0, y0, width, height);
    display_->setColor(WHITE);
    display_->display();
}