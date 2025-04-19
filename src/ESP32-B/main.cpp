#include <Arduino.h>
#include "Motor.h"
#include "motor_comm.h"

// ------------------- Global MotorControl Instance -------------------
Motor motor;
 
// ------------------- Setup and Loop -------------------
void setup() {
  motor.begin();
  motor.setSpeed(3450);     // 200 steps/sec = 60 RPM
  motor.setRunning(false);  // Start motor immediately
  setupMotorComm(motor);
}
 
void loop() {
  motor.stepMotor();
  updateSerialReceiver(motor);
}