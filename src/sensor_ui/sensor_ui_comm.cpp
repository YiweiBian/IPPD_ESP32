#include <Arduino.h>
#include "sensor_ui_comm.h"

#define UART_TX 17  // To RX on Motor ESP32
#define UART_RX 16  // Not used for now
#define BUTTON_1 12 // Start
#define BUTTON_2 14 // Increase freq
#define BUTTON_3 27 // Decrease freq
#define BUTTON_4 26 // Stop

HardwareSerial motorSerial(2); // UART2

void setupComm()
{
  Serial.begin(115200); // USB serial monitor
  motorSerial.begin(9600, SERIAL_8N1, UART_RX, UART_TX);
  Serial.println("🔵 Sensor/UI ESP32: UART2 Initialized");
}

void sendTestCommands()
{
  static unsigned long lastSent = 0;
  static int state = 0;

  if (millis() - lastSent > 3000)
  {
    String command;

    switch (state)
    {
    case 0:
      command = "START\n";
      break;
    case 1:
      command = "FREQ=800\n";
      break;
    case 2:
      command = "DIR\n";
      break;
    case 3:
      command = "STOP\n";
      break;
    default:
      state = -1;
      return;
    }

    motorSerial.print(command);
    Serial.print("Sent to motor: ");
    Serial.print(command);

    lastSent = millis();
    state++;
  }
}

const int buttonPins[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};

void setupButtons()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void handleRemoteButtons(HardwareSerial &motorSerial)
{
  for (int i = 0; i < 4; i++)
  {
    bool currentState = digitalRead(buttonPins[i]);
    if (lastButtonState[i] == HIGH && currentState == LOW)
    {
      switch (i)
      {
      case 0:
        motorSerial.print("START\n");
        Serial.println("Remote: START");
        break;
      case 1:
        motorSerial.print("INC\n");
        Serial.println("Remote: INC");
        break;
      case 2:
        motorSerial.print("DEC\n");
        Serial.println("Remote: DEC");
        break;
      case 3:
        motorSerial.print("STOP\n");
        Serial.println("Remote: STOP");
        break;
      }
    }
    lastButtonState[i] = currentState;
  }
}
