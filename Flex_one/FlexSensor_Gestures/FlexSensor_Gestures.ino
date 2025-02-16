#include "gestures.h"

const int flexSensorPins[5] = {A0, A1, A2, A3, A4};  // Analog pins for 5 flex sensors
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;  // 15kΩ pull-up resistor

float baselineResistance[5];  // Store initial resistance for calibration

const float resistanceToAngleFactor = 0.05;  
const float STRAIGHT_THRESHOLD = 45.0;
const float PARTIALLY_BENT_THRESHOLD = 180.0;

void setup() {
    Serial.begin(115200);
    Serial.println("Calibrating...");

    for (int i = 0; i < 5; i++) {
        float sumResistance = 0;
        int samples = 10;

        for (int j = 0; j < samples; j++) {
            sumResistance += readFlexSensorResistance(flexSensorPins[i]);
            delay(50);
        }

        baselineResistance[i] = sumResistance / samples;
    }

    Serial.println("Calibration Complete.\n");
    delay(2000);
}

void loop() {
    float angles[5];
    String bendStates[5];

    for (int i = 0; i < 5; i++) {
        float currentResistance = readFlexSensorResistance(flexSensorPins[i]);
        float deltaResistance = currentResistance - baselineResistance[i];
        angles[i] = abs(deltaResistance) * resistanceToAngleFactor;

        if (angles[i] < STRAIGHT_THRESHOLD) {
            bendStates[i] = "S";
        } else if (angles[i] >= STRAIGHT_THRESHOLD && angles[i] < PARTIALLY_BENT_THRESHOLD) {
            bendStates[i] = "P";
        }
    }

    // Create gesture string
    String gestureString = "";
    for (int i = 0; i < 5; i++) {
        gestureString += bendStates[i];
        if (i < 4) gestureString += " ";
    }

    // Recognize the gesture
    String detectedGesture = recognizeGesture(gestureString);

    // Print output
    Serial.print("Gesture: ");
    Serial.print(gestureString);
    Serial.print(" -> ");
    Serial.println(detectedGesture);

    delay(500);
}

// Function to calculate flex sensor resistance
float readFlexSensorResistance(int pin) {
    int samples = 10;
    float total = 0;

    for (int i = 0; i < samples; i++) {
        total += analogRead(pin);
        delay(5);
    }

    float sensorValue = total / samples;
    if (sensorValue == 0) return 10000;

    float voltage = sensorValue * (supplyVoltage / 1023.0);
    float resistance = (pullUpResistor * (supplyVoltage - voltage)) / voltage;
    return resistance;
}
