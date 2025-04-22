#include "Uart.h"
#include "Nextion.h"
#include "Sensor.h"
#include "Keypad.h"
#include <Arduino.h>

// Define hardware module classes
Sensor sensor(500);
Keypad keypad;
Nextion nextion(Serial1);

// Nextion update states
unsigned long lastNextionUpdate = 0;
const unsigned long nextionUpdateInterval = 500;

// motor control states
bool isRunning = false;
uint16_t pumpFrequency = 2415;
uint16_t stepIncrement = 345;
uint16_t PUMP_MAX = 10350;

// ESP32-ESP32 communiation protocol
enum CommandType : uint8_t {
  CMD_NONE = 0x0,
  CMD_RUN = 0x1,
  CMD_STOP = 0x2,
  CMD_SET = 0x3
};

void handleCommand(String cmd);
void handleKeypad(int cmd);
void check();
void sendCommandPacket(CommandType cmd, uint16_t value);

// interrupt service routine for sensor
void IRAM_ATTR onPulse() { sensor.handlePulse(); }

void setup()
{
  setupComm(); // Initialize UART2
  Serial1.begin(115200);
  nextion.begin();
  sensor.begin();
  keypad.begin();
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), onPulse, RISING);
}

void loop()
{
  sensor.updateFrequency();
  String cmd = nextion.getCommand();
  Serial.print(cmd);
  if (!cmd.isEmpty())
    handleCommand(cmd);
  handleKeypad(keypad.read());
  check();

  // update Nextion every 500 ms
  if (millis() - lastNextionUpdate >= nextionUpdateInterval)
  {
    nextion.sendStatus(isRunning, sensor.getFlowRate(), pumpFrequency);
    lastNextionUpdate = millis();
  }
}

void handleCommand(String cmd)
{
  if (cmd == "RUN")
      {
        isRunning = true;
        sendCommandPacket(CMD_RUN, 0);
      }
      else if (cmd == "STO")
      {
        isRunning = false;
        sendCommandPacket(CMD_STOP, 0);
      }
      else if (cmd == "ABL")
      {
        PUMP_MAX = 15000; // 140mL/min
      }
      else if (cmd == "BLZ")
      {
        PUMP_MAX = 2415; // 40mL/min
      }
      else if (cmd == "INC")
      {
        if (pumpFrequency < PUMP_MAX)
        {
          pumpFrequency += stepIncrement;
          sendCommandPacket(CMD_SET, pumpFrequency);
        }
      }
      else if (cmd == "DEC")
      {
        if (pumpFrequency > stepIncrement)
        {
          pumpFrequency -= stepIncrement;
          sendCommandPacket(CMD_SET, pumpFrequency);
        }
      }
}
void handleKeypad(int cmd)
{ 
switch (cmd) {
  // case 0:
  //   isRunning = true;
  //   sendCommandPacket(CMD_RUN, 0);
  //   break;
  
  case 3: // increse button

    // increase the pumpFrequency by predefined step
    if (pumpFrequency < PUMP_MAX)
      pumpFrequency += stepIncrement;

    // send the increased pumpFrequency value to ESP32-B 
    sendCommandPacket(CMD_SET, pumpFrequency);
    break;

  case 2: // decrease button

    // decrease the pumpFrequency by predefined step
    if (pumpFrequency > stepIncrement)
      pumpFrequency -= stepIncrement;

    // send the decreased pumpFrequency value to ESP32-B
    sendCommandPacket(CMD_SET, pumpFrequency);
    break;

  case 1: // run/stop toggle button
    isRunning = !isRunning;
    sendCommandPacket((isRunning ? CMD_RUN : CMD_STOP), pumpFrequency);
    break;
  }
}
void check()
{

}

void sendCommandPacket(CommandType cmd, uint16_t value) 
{
  // encode the command and the value into 2-byte instruction
  // (More detail included in design report)
  uint8_t byte1, byte2;
  byte1 = (cmd << 6) | ((value >> 8) & 0x3F);
  byte2 = value & 0xFF;
  
  // send the encoded instruction
  writeCommand(byte1);
  writeCommand(byte2);
}