const int flexSensorPins[5] = {A0, A1, A2, A3, A4};  // Analog pins for 5 flex sensors
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;  // 15kΩ pull-up resistor

float minResistance[5];  // Store minimum resistance (straight fingers)
float maxResistance[5];  // Store maximum resistance (fully bent fingers)
const float resistanceToAngleFactor = 0.05;  // Conversion factor

void setup() {
    Serial.begin(115200);
    calibrateFlexSensors();
}

void loop() {
    float angles[5];
    String bendStates[5];

    for (int i = 0; i < 5; i++) {
        float currentResistance = readFlexSensorResistance(flexSensorPins[i]);
        float normalizedAngle = map(currentResistance, minResistance[i], maxResistance[i], 0, 180);
        normalizedAngle = constrain(normalizedAngle, 0, 180);
        angles[i] = normalizedAngle;

        // Determine bending position
        if (angles[i] < 45) {
            bendStates[i] = "S"; // Straight
        } else {
            bendStates[i] = "B"; // Bent
        }
    }

    // Print Serial Output
    Serial.print("Angle: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(angles[i], 1);
        if (i < 4) Serial.print(", ");
    }
    
    Serial.print("\nBend: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(bendStates[i]);
        if (i < 4) Serial.print(", ");
    }
    Serial.println("\n");

    delay(500);
}

void calibrateFlexSensors() {
    Serial.println("Keep your fingers STRAIGHT and press any key to continue...");
    while (Serial.available() > 0) Serial.read(); // Clear buffer
    while (Serial.available() == 0) {}
    Serial.read();
    calibrateResistance(minResistance);
    Serial.println("Straight position recorded.\n");

    Serial.println("Now BEND your fingers fully and press any key to continue...");
    while (Serial.available() > 0) Serial.read(); // Clear buffer
    while (Serial.available() == 0) {}
    Serial.read();
    calibrateResistance(maxResistance);
    Serial.println("Bent position recorded.\n");
}

void calibrateResistance(float resistanceArray[5]) {
    for (int i = 0; i < 5; i++) {
        float sumResistance = 0;
        int samples = 10;

        for (int j = 0; j < samples; j++) {
            sumResistance += readFlexSensorResistance(flexSensorPins[i]);
            delay(50);
        }

        resistanceArray[i] = sumResistance / samples;
        Serial.print("Sensor ");
        Serial.print(i + 1);
        Serial.print(" Resistance: ");
        Serial.print(resistanceArray[i]);
        Serial.println(" Ω");
    }
}

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
