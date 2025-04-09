#include <Arduino.h>
#include "motor_logic.h"
#include "motor_comm.h"

void setup()
{
  motor.begin();
  motor.setSpeed(1000);   // Default speed
  motor.setRunning(true); // Start motor

  setupMotorComm(); // Initialize UART communication
}

void loop()
{
  motor.stepMotor();     // Handle stepping
  handleMotorCommands(); // Handle incoming UART commands
}
