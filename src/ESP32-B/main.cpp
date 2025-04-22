#include <Arduino.h>
#include "Motor.h"
#include "motor_comm.h"

Motor motor;
 
void setup() {
  motor.begin();
  motor.setSpeed(3450);     // 200 steps/sec = 60 RPM
  motor.setRunning(false);
  setupMotorComm(motor);
}
 
void loop() {
  motor.stepMotor();  // control motor
  updateSerialReceiver(motor); // receive command from ESP32-A
}