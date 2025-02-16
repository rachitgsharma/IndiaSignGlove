#ifndef GESTURES_H
#define GESTURES_H

#include <Arduino.h>

// Define gesture patterns
struct Gesture {
    String pattern;
    String meaning;
};

// List of known gestures
const Gesture gestures[] = {
    {"P S P P S", "YO"},
    {"S S S S S", "OPEN HAND"},
    {"P P P P P", "CLOSED FIST"},
    {"P S S S P", "PEACE SIGN"},
    {"S P P P S", "THUMBS UP"}
};

// Number of gestures
const int NUM_GESTURES = sizeof(gestures) / sizeof(gestures[0]);

// Function to recognize gestures
String recognizeGesture(String currentGesture) {
    for (int i = 0; i < NUM_GESTURES; i++) {
        if (gestures[i].pattern == currentGesture) {
            return gestures[i].meaning;
        }
    }
    return "UNKNOWN";  // If no match is found
}

#endif
