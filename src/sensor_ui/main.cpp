#include <Arduino.h>
#include "sensor_ui_comm.h"

void setup()
{
  setupComm(); // Initialize UART2
}

void loop()
{
  sendTestCommands(); // Periodically send commands to Motor ESP32
}
