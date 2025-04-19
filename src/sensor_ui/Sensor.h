#pragma once
#include <Arduino.h>
#include "PINOUT.h"

// const unsigned long sensorInterval = 500; // 500ms interval
// float sensorFrequency = 0.0;

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
