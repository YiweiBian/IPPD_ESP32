#pragma once
#include <Arduino.h>

// Declare the class
class MotorControl
{
public:
  bool isRunning;
  unsigned long stepCount;
  int currentSpeed;

  MotorControl();
  void begin();
  void stepMotor();
  void setRunning(bool run);
  void reverseDirection();
  void setSpeed(int speed);

private:
  unsigned long stepDelayUs;
  void updateStepDelay();
};

// Declare the global motor object
extern MotorControl motor;
