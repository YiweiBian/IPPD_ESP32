#pragma once
#include <Arduino.h>

// ----------------------- Sensor Setup -----------------------
#define SENSOR_PIN 4
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
private:
    unsigned long interval;
    float frequency;
    volatile unsigned long pulseCount;
    unsigned long lastUpdate;
};
