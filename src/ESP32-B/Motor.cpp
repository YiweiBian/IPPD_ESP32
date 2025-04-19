#include "Motor.h"

// Implement the methods
Motor::Motor()
{
  isRunning = false;
  stepCount = 0;
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
  if (isRunning)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayUs);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayUs);
    stepCount++;
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
  stepDelayUs = 1000000UL / (2UL * currentSpeed);
}

void Motor::debug()
{
  Serial.println("--- Motor Status Debug ---");
  Serial.printf("isRunning: %d\n", isRunning);
  Serial.printf("currentSpeed: %d\n", currentSpeed);
}