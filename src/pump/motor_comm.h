#pragma once
#include "Motor.h"
#include "PINOUT.h"

void setupMotorComm(Motor& motor);      // Initializes UART
void handleMotorCommands(Motor& motor); // Checks UART for incoming messages
void updateSerialReceiver(Motor& motor);