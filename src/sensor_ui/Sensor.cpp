#include "Sensor.h"

Sensor::Sensor(const unsigned long si)
    : interval(si), frequency(0), pulseCount(0), lastUpdate(0) {}

void Sensor::begin()
{
    pinMode(SENSOR_PIN, INPUT);
}

void Sensor::handlePulse()
{
    pulseCount++;
}

float Sensor::updateFrequency()
{
    if (millis() - lastUpdate >= interval) {
        noInterrupts();
        unsigned long count = pulseCount;
        pulseCount = 0;
        interrupts();

        frequency = count * (1000.0 / interval);
        lastUpdate = millis();
    }
    // Serial.print("Sensor Reading: ");
    // Serial.println(frequency);
    return frequency;
}

float Sensor::getFrequency() const
{
    return frequency;
}

float Sensor::getFlowRate() const
{
    return frequency * 3;
}