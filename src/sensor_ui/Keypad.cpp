#include "Keypad.h"

Keypad::Keypad() {
    lastStates = new bool[BUTTON_COUNT];
    for (int i = 0; i < BUTTON_COUNT; i++)
        lastStates[i] = HIGH;
}

void Keypad::begin() {
    for (int i = 0; i < BUTTON_COUNT; i++)
        pinMode(buttonPins[i], INPUT_PULLUP);
}

int Keypad::read() {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        bool state = digitalRead(buttonPins[i]);
        if (lastStates[i] == HIGH && state == LOW) {
            lastStates[i] = state;
            return i;
        }
        lastStates[i] = state;
    }
    return -1;
}