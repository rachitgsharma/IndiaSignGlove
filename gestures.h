#ifndef GESTURES_H
#define GESTURES_H

#include <Arduino.h>

// Define gesture patterns with B (Bent) and S (Straight)
struct Gesture {
    String pattern;
    String meaning;
};

// List of known gestures with bend and touch
const Gesture gestures[] = {
    {"S B S S S | 0 0 0 0", "છે"},
    {"S S S S S | 0 0 0 0", "આવજો"},
    {"S S S S S | 1 0 0 1", "પાણી"},//
    {"S B B B B | 0 0 0 0", "પીવું"},
    {"S S B S S | 0 0 0 0", "મને"},
    {"B S S S S | 0 0 0 0", "હેલો"},
    {"S B B S S | 0 0 0 0", "કેમ"},
    {"S S S S S | 0 1 0 0", "ખાવું"},//CHECK
    {"B B B B B | 0 0 0 0", "છો"},
    {"B B S S S | 1 0 0 0", "ઠીક"},//CHECK
    {"B B S S S | 0 0 0 0", "તમે"},
    {"B S B B B | 0 0 0 0", "હું"},
    {"S S S S S | 0 0 1 1", "આભાર"},
    {"S S S B B | 0 0 0 0", "કૃપા કરીને"},
    {"S S S S S | 0 1 1 0", "બીમાર"},//CHECK
    {"S B B B S | 0 0 0 0", "હા"},
    {"S S B B S | 0 0 0 0", "મદદ કરો"},
    {"B S B S S | 0 0 0 0", "બહેરી"},
    {"B S B S S | 0 1 0 0", "મૂંગી"}

};

const int NUM_GESTURES = sizeof(gestures) / sizeof(gestures[0]);

// Gesture recognition function
String recognizeGesture(String currentGesture) {
    for (int i = 0; i < NUM_GESTURES; i++) {
        if (gestures[i].pattern == currentGesture) {
            return gestures[i].meaning;
        }
    }
    return "UNKNOWN";
}

#endif
