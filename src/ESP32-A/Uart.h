#pragma once
#include <Arduino.h>
#include "PINOUT.h"

void setupComm();
void sendCommand(String cmd);
void writeCommand(uint8_t cmd);