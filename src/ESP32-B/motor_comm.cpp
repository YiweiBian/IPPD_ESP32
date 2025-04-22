#include <Arduino.h>
#include "motor_comm.h"

enum CommandType : uint8_t {
  CMD_NONE = 0x0,
  CMD_RUN  = 0x1,
  CMD_STOP = 0x2,
  CMD_SET  = 0x3
};

struct CommandPacket {
  uint8_t command;  // 2-bit command
  uint16_t value;   // 14-bit value
};

static uint8_t packetBuffer[2]; // Buffer to store incoming bytes
static uint8_t packetIndex = 0; // How many bytes have been received so far

HardwareSerial espComm(2); // UART2

void setupMotorComm(Motor& motor)
{
  espComm.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
}

void updateSerialReceiver(Motor& motor) {
  // Read any available bytes
  if (espComm.available() && packetIndex < 2) {
    packetBuffer[packetIndex++] = espComm.read();
  }
  
  // When we have a full packet, decode it
  if (packetIndex == 2) {
    CommandPacket packet;
    // Extract the command from the upper 6 bits of the first byte
    packet.command = packetBuffer[0] >> 6;
    // Reconstruct the value from the lower 6 bits of byte1 and the full byte2.
    packet.value = ((uint16_t)(packetBuffer[0] & 0x3F) << 8) | packetBuffer[1];
    // Process the command.
    switch (packet.command) {
      case CMD_RUN:
        motor.setRunning(true);
        break;
        
      case CMD_STOP:
        motor.setRunning(false);
        break;
        
      case CMD_SET:
        motor.setSpeed(packet.value);
        break;
    }

    // Reset the index for the next packet.
    packetIndex = 0;
  }
}