#include <Arduino.h>
#include "motor_comm.h"
#include "motor_logic.h" // So we can access the global `motor` object

#define UART_RX 16
#define UART_TX 17

HardwareSerial espComm(2); // UART2

void setupMotorComm()
{
  espComm.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  Serial.begin(115200);
  Serial.println("🟢 MotorComm: UART2 Initialized");
}

void handleMotorCommands()
{
  if (espComm.available())
  {
    String cmd = espComm.readStringUntil('\n');
    cmd.trim();
    Serial.print("Received: ");
    Serial.println(cmd);

    if (cmd == "START")
    {
      motor.setRunning(true);
    }
    else if (cmd == "STOP")
    {
      motor.setRunning(false);
    }
    else if (cmd == "DIR")
    {
      motor.reverseDirection();
    }
    else if (cmd.startsWith("FREQ="))
    {
      int val = cmd.substring(5).toInt();
      motor.setSpeed(val);
    }
    else
    {
      Serial.println("⚠️ Unknown command");
    }
  }
}
