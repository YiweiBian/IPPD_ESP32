#include "Keypad.h"

Keypad::Keypad() {
    lastStates = new bool[BUTTON_COUNT]; // initiate lastStates array
    for (int i = 0; i < BUTTON_COUNT; i++)
        lastStates[i] = HIGH;
}

void Keypad::begin() {
    for (int i = 0; i < BUTTON_COUNT; i++)
        pinMode(buttonPins[i], INPUT_PULLUP);
}

int Keypad::read() {
    for (int i = 0; i < BUTTON_COUNT; i++) // check every keypad
    {
        bool state = digitalRead(buttonPins[i]); // read the keypad input

        if (lastStates[i] == HIGH && state == LOW) // if pressed
        {
            lastStates[i] = state; // change the state
            return i;
        }
        lastStates[i] = state;
    }
    return -1;
}