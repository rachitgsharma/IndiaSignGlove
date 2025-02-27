const int flexSensorPin = A0;  // Analog pin connected to flex sensor
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;  // 15kΩ pull-up resistor

float baselineResistance;  // Store initial resistance (for calibration)

// Calibration factor (adjust for sensitivity)
const float resistanceToAngleFactor = 0.01;  

// Define new threshold angles based on observed values
const float STRAIGHT_THRESHOLD = 45.0;   // Less than this → Straight
const float PARTIALLY_BENT_THRESHOLD = 100.0;  // Less than this → Partially Bent

void setup() {
    Serial.begin(9600);

    // Calibrate baseline resistance at startup
    baselineResistance = readFlexSensorResistance();
    Serial.print("Baseline Resistance: ");
    Serial.print(baselineResistance);
    Serial.println(" Ω");
    delay(2000);
}

void loop() {
    float currentResistance = readFlexSensorResistance();
    float deltaResistance = currentResistance - baselineResistance;

    // Ensure deltaResistance is always positive
    float angle = abs(deltaResistance) * resistanceToAngleFactor;  // Convert resistance change to angle

    Serial.print("Resistance: ");
    Serial.print(currentResistance);
    Serial.print(" Ω | Angle: ");
    Serial.print(angle);
    Serial.print("° -> Position: ");

    // Classify finger bending position
    if (angle < STRAIGHT_THRESHOLD) {
        Serial.println("Straight");
    } 
    else if (angle >= STRAIGHT_THRESHOLD && angle < PARTIALLY_BENT_THRESHOLD) {
        Serial.println("Partially Bent");
    } 
    else {
        Serial.println("Fully Bent");
    }

    delay(500);  // Log data every 500ms
}

// Function to calculate flex sensor resistance with pull-up resistor
float readFlexSensorResistance() {
    int sensorValue = analogRead(flexSensorPin);
    Serial.println(sensorValue);
    float voltage = sensorValue * (supplyVoltage / 1023.0);
    float resistance = (pullUpResistor * (supplyVoltage - voltage)) / voltage;
    return resistance;
}
