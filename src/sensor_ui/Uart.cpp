#include <Arduino.h>
#include "UART.h"

HardwareSerial motorSerial(2); // UART2

void setupComm()
{
  // Serial.begin(115200); // USB serial monitor
  motorSerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  // Serial.println("🔵 Sensor/UI ESP32: UART2 Initialized");
}

void sendCommand(String command)
{
  motorSerial.print(command + "\n");
  Serial.print("Nextion: " + command + "\n");
}

void writeCommand(uint8_t byte)
{
  motorSerial.write(byte);
  Serial.print("Byte: " + String(byte) + "\n");
}