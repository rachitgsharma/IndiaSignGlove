const int flexSensorPins[5] = {A0, A1, A2, A3, A4};  // Analog pins for 5 flex sensors
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;  // 15kΩ pull-up resistor

float baselineResistance[5];  // Store initial resistance for calibration

// Calibration factor (adjust based on flex sensor response)
const float resistanceToAngleFactor = 0.05;  

// Thresholds for classifying finger positions
const float STRAIGHT_THRESHOLD = 45.0;   // Less than this → Straight
const float PARTIALLY_BENT_THRESHOLD = 180.0;  // Less than this → Partially Bent

void setup() {
    Serial.begin(115200);
    Serial.println("Calibrating...");

    for (int i = 0; i < 5; i++) {
        float sumResistance = 0;
        int samples = 10;  // Take 10 samples for better accuracy

        for (int j = 0; j < samples; j++) {
            sumResistance += readFlexSensorResistance(flexSensorPins[i]);
            delay(50);
        }

        baselineResistance[i] = sumResistance / samples;

        Serial.print("Baseline Resistance F");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(baselineResistance[i]);
        Serial.println(" Ω");
    }

    Serial.println("Calibration Complete.\n");
    delay(2000);
}

void loop() {
    float angles[5];
    String bendStates[5];

    // Read and process each flex sensor
    for (int i = 0; i < 5; i++) {
        float currentResistance = readFlexSensorResistance(flexSensorPins[i]);
        float deltaResistance = currentResistance - baselineResistance[i];
        angles[i] = abs(deltaResistance) * resistanceToAngleFactor;  // Convert resistance change to angle

        // Determine bending position
        if (angles[i] < STRAIGHT_THRESHOLD) {
            bendStates[i] = "S";
        } else if (angles[i] >= STRAIGHT_THRESHOLD && angles[i] < PARTIALLY_BENT_THRESHOLD) {
            bendStates[i] = "P";
        }
    }

    // Print Serial Output in Requested Format
    Serial.print("Angle: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(angles[i], 1);  // Print 1 decimal precision
        if (i < 4) Serial.print(", ");
    }
    
    Serial.print("\nBend: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(bendStates[i]);
        if (i < 4) Serial.print(", ");
    }
    
    Serial.println("\n");

    delay(500);  // Log data every 500ms
}

// Function to calculate flex sensor resistance
float readFlexSensorResistance(int pin) {
    int samples = 10;
    float total = 0;

    for (int i = 0; i < samples; i++) {
        total += analogRead(pin);
        delay(5);
    }

    float sensorValue = total / samples;  // Average value
    if (sensorValue == 0) return 10000;

    float voltage = sensorValue * (supplyVoltage / 1023.0);
    float resistance = (pullUpResistor * (supplyVoltage - voltage)) / voltage;
    return resistance;
}

