#pragma once
#include <Arduino.h>
#include "PINOUT.h"

#define NEXTION_BUF_SIZE 128

class Nextion
{
public:
    Nextion(HardwareSerial& port);
    void begin();
    String getCommand();
    void sendStatus(bool isRunning, float sensorFreq, float motorFreq);
private:
    HardwareSerial& port;
    uint8_t buf[NEXTION_BUF_SIZE];
    size_t bufIndex;
};