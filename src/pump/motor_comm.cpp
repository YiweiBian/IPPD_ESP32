#include <Arduino.h>
#include "motor_comm.h"

#define UART_RX 16
#define UART_TX 17

enum CommandType : uint8_t {
  CMD_NONE     = 0x0, // No command / uninitialized
  CMD_RUN = 0x1, // Increase command
  CMD_STOP = 0x2, // Decrease command
  CMD_SET      = 0x3  // Set command with a value
};

struct CommandPacket {
  uint8_t command;  // extracted 4-bit command
  uint16_t value;   // reconstructed 12 or 14-bit value
};

// --- Variables for Non-blocking Serial Reading ---
static uint8_t packetBuffer[2]; // Buffer to store incoming bytes
static uint8_t packetIndex = 0; // How many bytes have been received so far

HardwareSerial espComm(2); // UART2

void setupMotorComm(Motor& motor)
{
  espComm.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  // Serial.begin(115200);
  // Serial.println("🟢 MotorComm: UART2 Initialized");
}

void updateSerialReceiver(Motor& motor) {
  // Read any available bytes without blocking.
  if (espComm.available() && packetIndex < 2) {
    // Serial.println("Hmm...");
    packetBuffer[packetIndex++] = espComm.read();
  }
  
  // When we have a full packet, process it.
  if (packetIndex == 2) {
    CommandPacket packet;
    // Extract the command from the upper 6 bits of the first byte.
    packet.command = packetBuffer[0] >> 6;
    // Reconstruct the value from the lower 6 bits of byte1 and the full byte2.
    packet.value = ((uint16_t)(packetBuffer[0] & 0x3F) << 8) | packetBuffer[1];
    // Serial.println("Received...");
    // Process the command.
    switch (packet.command) {
      case CMD_RUN:
        motor.setRunning(true);
        // Serial.println("RUN");
        // motor.debug();
        break;
        
      case CMD_STOP:
        motor.setRunning(false);
        // Serial.println("STOP");
        // motor.debug();
        break;
        
      case CMD_SET:
        motor.setSpeed(packet.value);
        // Serial.print("SET");
        // Serial.println(packet.value);
        // motor.debug();
        break;
        
      default:
        // Serial.println("Unknown command received.");
        break;
    }
    // Reset the index for the next packet.
    packetIndex = 0;
  }
}

void handleMotorCommands(Motor& motor)
{
  if (espComm.available())
  {
    String cmd = espComm.readStringUntil('\n');
    cmd.trim();
    Serial.print("Received: ");
    Serial.println(cmd);

    if (cmd == "RUN")
    {
      motor.setRunning(true);
    }
    else if (cmd == "STO")
    {
      motor.setRunning(false);
    }
    // else if ()
    // {
    //   motor.reverseDirection();
    // }
    // else if (cmd.startsWith("FREQ="))
    // {
    //   int val = cmd.substring(5).toInt();
    //   motor.setSpeed(val);
    // }
    // else
    // {
    //   Serial.println("⚠️ Unknown command");
    // }
  }
}
