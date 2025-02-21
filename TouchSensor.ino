// Constants for button pins
const int thumbPin = 2;
const int indexPin = 3;
const int middlePin = 4;
const int ringPin = 5;
const int pinkyPin = 6;

// Variables for button states
int thumbState = 0;
int indexState = 0;
int middleState = 0;
int ringState = 0;
int pinkyState = 0;

void setup() {
    Serial.begin(9600);
    pinMode(thumbPin, OUTPUT);
    digitalWrite(thumbPin, HIGH);
    pinMode(indexPin, INPUT);
    pinMode(middlePin, INPUT);
    pinMode(ringPin, INPUT);
    pinMode(pinkyPin, INPUT);
}

int readStableState(int pin) {
    int stableState = digitalRead(pin);
    delay(10); // Small debounce delay
    if (digitalRead(pin) == stableState) {
        return stableState;
    }
    return LOW; // Default to LOW if unstable
}

void loop() {
    // Read button states with stability check
    indexState = readStableState(indexPin);
    middleState = readStableState(middlePin);
    ringState = readStableState(ringPin);
    pinkyState = readStableState(pinkyPin);

    // Print individual finger states
    Serial.print(" Index: "); Serial.print(indexState);
    Serial.print(" | Middle: "); Serial.print(middleState);
    Serial.print(" | Ring: "); Serial.print(ringState);
    Serial.print(" | Pinky: "); Serial.println(pinkyState);

    // Check if all fingers are touching the thumb
    if (thumbState == HIGH && indexState == HIGH && middleState == HIGH && ringState == HIGH && pinkyState == HIGH) {
        Serial.println("All fingers touching the thumb - Gesture detected!");
    }

    delay(500);
}
