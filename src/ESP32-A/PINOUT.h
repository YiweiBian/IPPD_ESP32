// ----------------------- Keypad Button PINOUTs -----------------------
#define BUTTON_1 12 // Start pump
#define BUTTON_2 14 // Increase frequency
#define BUTTON_3 27 // Decrease frequency
#define BUTTON_4 26 // Stop pump
#define BUTTON_COUNT 4

// ----------------------- Sensor Pinouts -----------------------
#define SENSOR_PIN 4

// ----------------------- UART Pinouts -----------------------
#define UART_TX 17  // Connected to ESP32-B UART Rx
#define UART_RX 16

// ----------------------- Nextion Setup -----------------------
// We'll use UART1 for Nextion using safe pins (avoid flash pins)
// Nextion TX -> ESP32 RX on GPIO32, Nextion RX -> ESP32 TX on GPIO33
#define NEXTION_RX_PIN 32 // ESP32 RX (receives from Nextion TX)
#define NEXTION_TX_PIN 33 // ESP32 TX (sends to Nextion RX)
