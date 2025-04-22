#include "Motor.h"

Motor::Motor()
{
  isRunning = false;
  currentSpeed = 200;
  updateStepDelay();
}

void Motor::begin()
{
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
}

void Motor::stepMotor()
{
  if (isRunning) // if running
  { // create a step pulse
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayUs);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayUs);
  }
}

void Motor::setRunning(bool run)
{
  isRunning = run;
}

void Motor::reverseDirection()
{
  digitalWrite(DIR_PIN, !digitalRead(DIR_PIN));
}

void Motor::setSpeed(int speed)
{
  if (speed <= 0)
    return;
  currentSpeed = speed;
  updateStepDelay();
}

void Motor::updateStepDelay()
{
  stepDelayUs = 1000000UL / (2UL * currentSpeed); // defined by manufacturer
}

// void Motor::debug()
// {
//   Serial.println("--- Motor Status Debug ---");
//   Serial.printf("isRunning: %d\n", isRunning);
//   Serial.printf("currentSpeed: %d\n", currentSpeed);
// }