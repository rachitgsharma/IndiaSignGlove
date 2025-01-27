const int flexSensorPin = A0;  // Analog pin connected to flex sensor
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;  // 15kΩ pull-up resistor

float baselineResistance;  // Store initial resistance (for calibration)

// Calibration factor (tweak for sensitivity)
const float resistanceToAngleFactor = 0.05;  

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
  float angle = deltaResistance * resistanceToAngleFactor;  // Convert to angle

  Serial.print("Resistance: ");
  Serial.print(currentResistance);
  Serial.print(" Ω | Angle: ");
  Serial.print(angle*-1);
  Serial.println("°");

  delay(500);  // Log data every 500ms
}

// Function to calculate flex sensor resistance with pull-up resistor
float readFlexSensorResistance() {
  int sensorValue = analogRead(flexSensorPin);
  float voltage = sensorValue * (supplyVoltage / 1023.0);
  float resistance = (pullUpResistor * (supplyVoltage - voltage)) / voltage;
  return resistance;
} 
