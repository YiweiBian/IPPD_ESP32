#pragma once
#include <Arduino.h>
#include "PINOUT.h"

const int buttonPins[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
class Keypad {
public:
    Keypad();
    void begin();
    int read();
private:
    bool* lastStates;
};