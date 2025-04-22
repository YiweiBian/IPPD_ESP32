#include <Arduino.h>
#include <TMCStepper.h>

// ----------------------- TMC2209 and Motor Setup -----------------------
// UART Pins for TMC2209 (adjust based on your ESP32 setup)
#define RX2 16 // ESP32 RX2 for TMC2209 (from driver TX)
#define TX2 17 // ESP32 TX2 for TMC2209 (to driver RX)

// TMC2209 Configuration
#define DRIVER_ADDRESS 0b00 // Set based on your wiring
#define R_SENSE 0.11        // Rsense resistor value

// Create TMC2209 driver instance using Serial2
TMC2209Stepper driver(&Serial2, R_SENSE, DRIVER_ADDRESS);

// ----------------------- Nextion Setup -----------------------
// We'll use UART1 for Nextion using safe pins (avoid flash pins)
// Nextion TX -> ESP32 RX on GPIO32, Nextion RX -> ESP32 TX on GPIO33
#define NEXTION_RX_PIN 32 // ESP32 RX (receives from Nextion TX)
#define NEXTION_TX_PIN 33 // ESP32 TX (sends to Nextion RX)
HardwareSerial nextion(1);

// ----------------------- Motor Control Pins -----------------------
#define STEP_PIN 5
#define DIR_PIN 18
#define ENABLE_PIN 25

// ----------------------- Motor Control Variables -----------------------
unsigned long stepDelayUs = 500; // Calculated from pumpFrequency (default = 1000 microsteps/s)
bool isRunning = false;          // Motor starts off (stopped)
bool motorDirection = false;     // Default motor direction
int pumpFrequency = 1000;        // Pump frequency in microsteps per second

// ----------------------- Constants for RPM Calculation -----------------------
#define FULL_STEPS_PER_REV 200 // Motor full steps per revolution

// These globals will be computed after driver initialization:
int microstepsPerRev = 0; // FULL_STEPS_PER_REV * driver.microsteps()
int stepIncrement = 0;    // Increase/decrease increment (in microsteps/s) per button press

// ----------------------- Sensor Setup -----------------------
#define SENSOR_PIN 4
volatile unsigned long pulseCount = 0;
const unsigned long sensorInterval = 500; // 500ms interval
float sensorFrequency = 0.0;

// ----------------------- Serial Input -----------------------
String inputString = "";
bool inputComplete = false;

// ----------------------- Keypad Button Setup -----------------------
#define BUTTON_1 12 // Start pump
#define BUTTON_2 14 // Increase frequency
#define BUTTON_3 27 // Decrease frequency
#define BUTTON_4 26 // Stop pump

const int buttonPins[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const char *buttonNames[] = {"Start", "Increase Frequency", "Decrease Frequency", "Stop"};
bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};

// ----------------------- Nextion Buffer Setup -----------------------
#define NEXTION_BUF_SIZE 128
uint8_t nextionBuf[NEXTION_BUF_SIZE];
size_t nextionBufIndex = 0;

unsigned long lastNextionUpdate = 0;
const unsigned long nextionUpdateInterval = 500;

// ----------------------- Step Debug Variables -----------------------
unsigned long lastStepPrintTime = 0;
unsigned long stepCount = 0;

// ----------------------- Interrupt Service Routine -----------------------
void IRAM_ATTR handlePulse()
{
  pulseCount++;
}

// ----------------------- Function Prototypes -----------------------
void stepMotor();
void reverseMotorDirection();
void read_sensor();
void debugTMC2209();
void handleSerialInput();
void handleNextionInput(); // Nextion handler using byte buffer
void handleKeypadInput();
void updateSensorFrequency();
void updateNextion();
void printStatus();
void setPumpFrequency(int freq);

// ----------------------- printStatus() -----------------------
// Displays current state on Serial Monitor (calculations based on driver's microstepping)
void printStatus()
{
  // RPM = (pumpFrequency * 60) / microstepsPerRev
  float rpm = (pumpFrequency * 60.0) / microstepsPerRev;

  Serial.println("----- CURRENT STATUS -----");
  Serial.println("Mode: (screen command ABL/BLZ)"); // Placeholder
  Serial.print("Motor: ");
  Serial.println(isRunning ? "RUNNING" : "STOPPED");
  Serial.print("Frequency: ");
  Serial.print(pumpFrequency);
  Serial.print(" microsteps/s (");
  Serial.print(rpm);
  Serial.println(" RPM)");
  Serial.print("Direction: ");
  Serial.println(motorDirection ? "FORWARD" : "REVERSE");
  Serial.print("Driver microstepping: ");
  Serial.println(driver.microsteps());
  Serial.print("Step Increment: ");
  Serial.print(stepIncrement);
  Serial.println(" microsteps/s per button press");
  Serial.println("--------------------------\n");
}

// ----------------------- Setup -----------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for Serial monitor

  // Initialize TMC2209 driver on Serial2
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);

  // Initialize Nextion on UART1 (GPIO32 & GPIO33)
  nextion.begin(9600, SERIAL_8N1, NEXTION_RX_PIN, NEXTION_TX_PIN);

  Serial.println("🔧 TMC2209 Pump Control with Nextion and Keypad Initialized.");
  Serial.println("Serial Commands:");
  Serial.println("  - m <microsteps>  (Set microstepping) [default: as set by hardware]");
  Serial.println("  - c <current_mA>  (Set motor current) [default: 1200 mA]");
  Serial.println("  - f <frequency_microsteps/s> (Set pump frequency in microsteps/s)");
  Serial.println("  - d               (Debug TMC2209)");
  Serial.println("  - s               (Stop motor)");
  Serial.println("  - r               (Resume motor)");
  Serial.println("  - dir             (Reverse motor direction)");
  Serial.println("  - sread           (Read sensor for 20 samples @ 0.5s intervals)");
  Serial.println("Keypad Buttons:");
  Serial.println("  - Button 1: Start pump");
  Serial.println("  - Button 2: Increase frequency (by 1/20 max RPM)");
  Serial.println("  - Button 3: Decrease frequency (by 1/20 max RPM)");
  Serial.println("  - Button 4: Stop pump");

  // Initialize TMC2209 driver (let hardware microstepping remain)
  driver.begin();
  driver.toff(5);
  driver.rms_current(1200); // Set RMS current to 1200 mA (safe for a 1.5A motor)
  // Do not override microsteps so that the driver remains in its default (256)

  // Compute microsteps per revolution based on driver's setting:
  microstepsPerRev = FULL_STEPS_PER_REV * driver.microsteps();
  // We want each button press to change RPM by 1/20 of max RPM (600 RPM / 20 = 30 RPM)
  // Convert 30 RPM to microsteps/s:
  // pumpFrequency = (RPM * microstepsPerRev) / 60  so for 30 RPM: (30 * microstepsPerRev) / 60
  stepIncrement = (30 * microstepsPerRev) / 60; // For 51200, that's 25600 microsteps/s

  // Setup motor control pins
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIR_PIN, motorDirection);

  // Setup sensor and attach interrupt
  pinMode(SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), handlePulse, RISING);

  // Setup keypad buttons
  for (int i = 0; i < 4; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Set default pump frequency and update step delay
  pumpFrequency = 1000; // Default frequency in microsteps/s
  stepDelayUs = 1000000UL / (pumpFrequency * 2);

  Serial.println("✅ Pump Control Ready.\n");
  printStatus();

  lastStepPrintTime = millis();
  stepCount = 0;
}

// ----------------------- Main Loop -----------------------
void loop()
{
  handleSerialInput();
  handleNextionInput();
  handleKeypadInput();
  stepMotor();

  if (millis() - lastStepPrintTime >= 5000)
  {
    float effectiveHz = stepCount / ((millis() - lastStepPrintTime) / 1000.0);
    Serial.print("Effective step frequency: ");
    Serial.print(effectiveHz);
    Serial.println(" Hz");
    lastStepPrintTime = millis();
    stepCount = 0;
  }
  if (millis() - lastNextionUpdate >= nextionUpdateInterval)
  {
    updateNextion();
    lastNextionUpdate = millis();
  }
}

void handleKeypadInput()
{
  for (int i = 0; i < 4; i++)
  {
    bool currentState = digitalRead(buttonPins[i]);
    if (lastButtonState[i] == HIGH && currentState == LOW)
    {
      switch (i)
      {
      case 0: // Button 1: Start pump
        isRunning = true;
        Serial.println("Pump started via keypad.");
        break;
      case 1: // Button 2: Increase frequency by stepIncrement
        setPumpFrequency(pumpFrequency + stepIncrement);
        break;
      case 2: // Button 3: Decrease frequency by stepIncrement
        if (pumpFrequency > stepIncrement)
          setPumpFrequency(pumpFrequency - stepIncrement);
        else
          Serial.println("Minimum pump frequency reached.");
        break;
      case 3: // Button 4: Stop pump
        isRunning = false;
        Serial.println("Pump stopped via keypad.");
        break;
      }
      printStatus();
    }
    lastButtonState[i] = currentState;
  }
}

void updateSensorFrequency()
{
  static unsigned long lastSensorMillis = 0;

  if (millis() - lastSensorMillis >= sensorInterval)
  {
    noInterrupts();
    unsigned long count = pulseCount;
    pulseCount = 0;
    interrupts();

    sensorFrequency = count * (1000.0 / sensorInterval); // Hz

    lastSensorMillis = millis();
  }
}

void updateNextion()
{
  String cmd = "t4.txt=\"";
  cmd += (isRunning ? "Running" : "Standby");
  cmd += "\"";
  nextion.print(cmd);
  nextion.write(0xFF);
  nextion.write(0xFF);
  nextion.write(0xFF);

  updateSensorFrequency(); // just keeps sensorFrequency updated

  cmd = "x0.val=";
  cmd += String((int)(sensorFrequency * 10));
  nextion.print(cmd);
  nextion.write(0xFF);
  nextion.write(0xFF);
  nextion.write(0xFF);
}

// ----------------------- setPumpFrequency() -----------------------
// pumpFrequency is in microsteps per second based on driver's microstepping.
void setPumpFrequency(int freq)
{
  int ms = driver.microsteps();                    // e.g., 256
  int microstepsPerRev = FULL_STEPS_PER_REV * ms;  // e.g., 200 * 256 = 51200
  int maxFreq = (600.0 / 60.0) * microstepsPerRev; // For 600 RPM, e.g., 10 * 51200 = 512000 microsteps/s
  if (freq > maxFreq)
  {
    Serial.print("RPM capped at 600. Frequency set to ");
    Serial.print(maxFreq);
    Serial.println(" microsteps/s.");
    freq = maxFreq;
  }
  int safeFreq = (400.0 / 60.0) * microstepsPerRev; // For 400 RPM, e.g., ~341333 microsteps/s
  if (freq > safeFreq && freq <= maxFreq)
  {
    Serial.println("Warning: Running above safe RPM (above 400 RPM)!");
  }
  if (freq <= 0)
  {
    Serial.println("⚠️ Frequency must be > 0.");
    return;
  }
  pumpFrequency = freq;
  stepDelayUs = 1000000UL / (pumpFrequency * 2);
  float rpm = (pumpFrequency * 60.0) / microstepsPerRev;
  Serial.print("Pump frequency set to: ");
  Serial.print(pumpFrequency);
  Serial.print(" microsteps/s (");
  Serial.print(rpm);
  Serial.println(" RPM)");
}

// ----------------------- stepMotor() -----------------------
void stepMotor()
{
  if (isRunning)
  {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayUs);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayUs);
    stepCount++;
  }
}

// ----------------------- reverseMotorDirection() -----------------------
void reverseMotorDirection()
{
  motorDirection = !motorDirection;
  digitalWrite(DIR_PIN, motorDirection);
  Serial.print("Motor direction reversed. Now: ");
  Serial.println(motorDirection ? "FORWARD" : "REVERSE");
}

// ----------------------- read_sensor() -----------------------
// Reads 20 samples at 0.5s intervals while stepping the motor.
void read_sensor()
{
  Serial.println("\n📡 Starting Sensor Readings (20 samples @ 0.5s intervals)...");
  int readings = 0;
  unsigned long lastSensorMillis = millis();
  while (readings < 20)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorMillis >= sensorInterval)
    {
      noInterrupts();
      unsigned long count = pulseCount;
      pulseCount = 0;
      interrupts();
      sensorFrequency = count * (1000.0 / sensorInterval);
      Serial.print("Reading ");
      Serial.print(readings + 1);
      Serial.print(": ");
      Serial.print(sensorFrequency);
      Serial.println(" Hz");
      readings++;
      lastSensorMillis = currentMillis;
    }
    stepMotor();
  }
  Serial.println("✅ Sensor Readings Complete.\n");
}

// ----------------------- debugTMC2209() -----------------------
// Prints current TMC2209 settings.
void debugTMC2209()
{
  Serial.println("\n🔍 Running TMC2209 Debug...");
  Serial.print("GCONF Register: 0x");
  Serial.println(driver.GCONF(), HEX);
  Serial.print("Microstepping (driver readback): ");
  Serial.println(driver.microsteps());
  Serial.print("Current Set To: ");
  Serial.print(driver.rms_current());
  Serial.println(" mA");
  Serial.print("IRUN Register (Current % of Max): ");
  Serial.println(driver.irun());
  Serial.println("✅ Debugging Complete.\n");
}
