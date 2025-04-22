#include <Arduino.h>
#include "UART.h"

HardwareSerial motorSerial(2); // UART2

void setupComm()
{
  motorSerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
}

void sendCommand(String command)
{
  motorSerial.print(command + "\n");
}

void writeCommand(uint8_t byte)
{
  motorSerial.write(byte);
}