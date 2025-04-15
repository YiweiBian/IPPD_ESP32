#include "sensor_ui_comm.h"
#include "Nextion.h"
#include "Sensor.h"
#include "Keypad.h"
#include <Arduino.h>

Sensor sensor(500);
Keypad keypad;

Nextion nextion(Serial1);  // UART1: GPIO32/33
HardwareSerial nextionSerial(1);
unsigned long lastNextionUpdate = 0;
const unsigned long nextionUpdateInterval = 500;

// motor control
bool isRunning = false;
uint16_t pumpFrequency = 1000;
uint16_t stepIncrement = 100;
// volatile bool frequencyChanged;

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
  if (millis() - lastNextionUpdate >= nextionUpdateInterval)
  {
    nextion.sendStatus(isRunning, sensor.getFrequency(), pumpFrequency);
    lastNextionUpdate = millis();
  }
}

void handleCommand(String cmd)
{
  if (cmd == "RUN")
      {
        isRunning = true;
        sendCommandPacket(CMD_RUN, 0);
        Serial.println("Motor started (Nextion).");
      }
      else if (cmd == "STOP")
      {
        isRunning = false;
        sendCommandPacket(CMD_STOP, 0);
        // sendCommand("STOP");
        Serial.println("Motor stopped (Nextion).");
      }
      // else if (cmd == "DIR")
      // {
      //   reverseMotorDirection();
      // }
      else if (cmd == "ABL")
      {
        Serial.println("Mode set to ABL (Nextion).");
      }
      else if (cmd == "BLZ")
      {
        Serial.println("Mode set to BLZ (Nextion).");
      }
      // else if (cmd.startsWith("FREQ="))
      // {
      //   int freqVal = cmd.substring(5).toInt();
      //   setPumpFrequency(freqVal);
      // }
      else if (cmd == "INC")
      {
        if (pumpFrequency < 2000)
        {
          pumpFrequency += stepIncrement;
          sendCommandPacket(CMD_SET, pumpFrequency);
          Serial.println("Frequency increased (Nextion).");
          // sendCommand(String(pumpFrequency));
        }
        else
        {
          Serial.println("Minimum pump frequency reached (Nextion).");
        }
      }
      else if (cmd == "DEC")
      {
        if (pumpFrequency > stepIncrement)
        {
          pumpFrequency -= stepIncrement;
          sendCommandPacket(CMD_SET, pumpFrequency);
          // sendCommand(String(pumpFrequency));
          Serial.println("Frequency decreased (Nextion).");
        }
        else
        {
          Serial.println("Minimum pump frequency reached (Nextion).");
        }
      }
      else
      {
        Serial.println("⚠️ Unknown Nextion command received.");
      }
}
void handleKeypad(int cmd)
{
  if (cmd != -1)
    // Serial.println(cmd);
  switch (cmd) {
  // case 0:
  //   isRunning = true;
  //   sendCommandPacket(CMD_RUN, 0);
  //   break;
  case 3:
    if (pumpFrequency < 10000)
      pumpFrequency += stepIncrement;
    else 
      Serial.println("Max reached! (Keypad)");
    sendCommandPacket(CMD_SET, pumpFrequency);
    break;
  case 2:
    if (pumpFrequency > stepIncrement)
      pumpFrequency -= stepIncrement;
    else 
      Serial.println("Min reached 0! (Keypad)");
    sendCommandPacket(CMD_SET, pumpFrequency);
    break;
  case 1:
    isRunning = !isRunning;
    sendCommandPacket((isRunning ? CMD_RUN : CMD_STOP), pumpFrequency);
    break;
  }
}
void check()
{

}

void sendCommandPacket(CommandType cmd, uint16_t value) {
  uint8_t byte1, byte2;
  byte1 = (cmd << 6) | ((value >> 8) & 0x3F);
  byte2 = value & 0xFF;
  
  writeCommand(byte1);
  writeCommand(byte2);

  Serial.print("Sent packet: 0x");
  Serial.print(byte1, HEX);
  Serial.print(" 0x");
  Serial.println(byte2, HEX);
}