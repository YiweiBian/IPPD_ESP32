#pragma once
#include <Arduino.h>
#include "PINOUT.h"

// Declare the class
class Motor
{
public:
  bool isRunning;
  int currentSpeed;

  Motor();
  void begin();
  void stepMotor();
  void setRunning(bool run);
  void reverseDirection();
  void setSpeed(int speed);
  // void debug();

private:
  unsigned long stepDelayUs;
  void updateStepDelay();
};