#pragma once
#include <Arduino.h>

// ----------------------- Keypad Button Setup -----------------------
#define BUTTON_1 12 // Start pump
#define BUTTON_2 14 // Increase frequency
#define BUTTON_3 27 // Decrease frequency
#define BUTTON_4 26 // Stop pump
#define BUTTON_COUNT 4

const int buttonPins[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
// const char *buttonNames[] = {"Start", "Increase Frequency", "Decrease Frequency", "Stop"};
// bool lastButtonState[4] = {HIGH, HIGH, HIGH, HIGH};

class Keypad {
public:
    Keypad();
    void begin();
    int read();
private:
    bool* lastStates;
};