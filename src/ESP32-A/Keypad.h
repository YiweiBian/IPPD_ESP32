#pragma once
#include <Arduino.h>
#include "PINOUT.h"

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