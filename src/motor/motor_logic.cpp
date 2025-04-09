#include "motor_logic.h"

#define STEP_PIN 5
#define DIR_PIN 18
#define ENABLE_PIN 25

// Define the global instance
MotorControl motor;

// Implement the methods
MotorControl::MotorControl()
{
  isRunning = false;
  stepCount = 0;
  currentSpeed = 200;
  updateStepDelay();
}

void MotorControl::begin()
{
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
}

void MotorControl::stepMotor()
{
  if (isRunning)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayUs);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayUs);
    stepCount++;
  }
}

void MotorControl::setRunning(bool run)
{
  isRunning = run;
}

void MotorControl::reverseDirection()
{
  digitalWrite(DIR_PIN, !digitalRead(DIR_PIN));
}

void MotorControl::setSpeed(int speed)
{
  if (speed <= 0)
    return;
  currentSpeed = speed;
  updateStepDelay();
}

void MotorControl::updateStepDelay()
{
  stepDelayUs = 1000000UL / (2UL * currentSpeed);
}
