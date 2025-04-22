#pragma once
#include <Arduino.h>
#include "PINOUT.h"
class Sensor
{
public:
    Sensor(const unsigned long si);
    void begin();
    void handlePulse();
    float updateFrequency();
    float getFrequency() const;
    float getFlowRate() const;
private:
    unsigned long interval;
    float frequency;
    volatile unsigned long pulseCount;
    unsigned long lastUpdate;
};
