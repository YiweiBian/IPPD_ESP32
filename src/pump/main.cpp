#include <Arduino.h>
#include "Motor.h"
#include "motor_comm.h"

// ------------------- Pin Definitions -------------------
#define STEP_PIN 5
#define DIR_PIN 18
#define ENABLE_PIN 25 
// ------------------- Global MotorControl Instance -------------------
Motor motor;
 
// ------------------- Setup and Loop -------------------
void setup() {
  motor.begin();
  motor.setSpeed(4000);     // 200 steps/sec = 60 RPM
  motor.setRunning(true);  // Start motor immediately
  // setupMotorComm(motor);
}
 
void loop() {
  motor.stepMotor();
  // updateSerialReceiver(motor);
}