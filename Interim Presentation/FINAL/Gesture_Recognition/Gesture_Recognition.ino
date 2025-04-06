#include <Wire.h>
#include <MPU6500_WE.h>
#include <math.h>

// Gyroscope
#define MPU6500_ADDR 0x68
MPU6500_WE mpu6500 = MPU6500_WE(MPU6500_ADDR);
float ax_offset = 0, ay_offset = 0, az_offset = 0;
float gx_offset = 0, gy_offset = 0, gz_offset = 0;

// Flex Sensors
const int flexSensorPins[5] = {A0, A1, A2, A3, A4};
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;
float minResistance[5];
float maxResistance[5];
const float resistanceToAngleFactor = 0.05;

// Touch Sensors
const int finger1Pin = 22;
const int finger2Pin = 23;
const int finger3Pin = 24;
const int finger4Pin = 25;

// Buttons
const int emergencyButtonPin = 8;
const int recordButtonPin = 9;

// Gesture Patterns
struct Gesture {
  String bendStates[5];
  int touchStates[4];
  String meaning;
};

Gesture gestures[] = {
  { {"S", "S", "S", "S", "S"}, {1, 1, 1, 1}, "હેલો"},
  { {"B", "B", "S", "S", "S"}, {1, 0, 0, 0}, "કેમ છો?"},
  { {"S", "S", "S", "B", "B"}, {0, 0, 0, 1}, "તમારું નામ, શું છે?"},
  { {"B", "S", "S", "B", "B"}, {0, 0, 1, 1}, "આવજો"},
  { {"B", "S", "S", "B", "S"}, {0, 0, 1, 0}, "આભાર"},
  { {"B", "S", "S", "S", "S"}, {0, 1, 1, 0}, "હા"},
  { {"S", "B", "S", "S", "S"}, {1, 1, 0, 0}, "ના"}
  };

// Data Structures
struct SensorData {
  int pitch;
  int roll;
  float angles[5];
  String bendStates[5];
  int touchStates[4];

  String toJSON() {
    String json = "{";
    json += "\"emergency\":" + String(digitalRead(emergencyButtonPin)) +",";
    json += "\"record\":" + String(digitalRead(recordButtonPin)) +",";
    json += "\"pitch\": " + String(pitch) + ", ";
    json += "\"roll\": " + String(roll) + ", ";
    json += "\"angles\": [";
    for (int i = 0; i < 5; i++) {
      json += String(angles[i]);
      if (i < 4) json += ", ";
    }
    json += "], \"bendStates\": [";
    for (int i = 0; i < 5; i++) {
      json += "\"" + bendStates[i] + "\"";
      if (i < 4) json += ", ";
    }
    json += "], \"touchStates\": [";
    for (int i = 0; i < 4; i++) {
      json += String(touchStates[i]);
      if (i < 3) json += ", ";
    }
    json += "]";
    json += "}";
    return json;
  }
};

// Timing
unsigned long previousMillis = 0;
const long interval = 100;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  mpu6500.enableGyrDLPF();
  mpu6500.setGyrDLPF(5);
  mpu6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  mpu6500.setAccRange(MPU6500_ACC_RANGE_4G);
  calibrateMPU();

  Serial.println("Calibrating Flex Sensors...");
  calibrateFlexSensors();
  Serial.println("Calibration Complete.\n");

  pinMode(finger1Pin, INPUT_PULLUP);
  pinMode(finger2Pin, INPUT_PULLUP);
  pinMode(finger3Pin, INPUT_PULLUP);
  pinMode(finger4Pin, INPUT_PULLUP);

  pinMode(emergencyButtonPin, INPUT);
  pinMode(recordButtonPin, INPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    SensorData sensorData = readSensors();
    Serial.println(sensorData.toJSON());
    recognizeGesture(sensorData);
  }
}

void recognizeGesture(SensorData data) {
  for (int i = 0; i < sizeof(gestures) / sizeof(gestures[0]); i++) {
    bool match1 = true;
    bool match2 = true;
    for (int j = 0; j < 5; j++) {
      if (data.bendStates[j] != gestures[i].bendStates[j]) {
        match1 = false;
        break;
      }
    }
    for (int j = 0; j < 4; j++) {
      if (data.touchStates[j] != gestures[i].touchStates[j]) {
        match2 = false;
        break;
      }
    }
    if (match1 || match2) {
      Serial.println("Recognized Gesture: " + gestures[i].meaning);
      return;
    }
  }
  Serial.println("Gesture not recognized.");
}

void calibrateMPU() {
  Serial.println("Calibrating MPU6500... Hold still.");
  int samples = 5;
  for (int i = 0; i < samples; i++) {
    xyzFloat acc = mpu6500.getGValues();
    xyzFloat gyro = mpu6500.getGyrValues();
    ax_offset += acc.x;
    ay_offset += acc.y;
    az_offset += acc.z - 1.0;
    gx_offset += gyro.x;
    gy_offset += gyro.y;
    gz_offset += gyro.z;
    delay(10);
  }
  ax_offset /= samples;
  ay_offset /= samples;
  az_offset /= samples;
  gx_offset /= samples;
  gy_offset /= samples;
  gz_offset /= samples;
  Serial.println("Calibration Complete!");
}

void calibrateFlexSensors() {
  Serial.println("Keep your fingers STRAIGHT and press any key to continue...");
  while (Serial.available() == 0) {}
  Serial.read();
  calibrateResistance(minResistance);
  Serial.println("Straight position recorded.\n");

  Serial.println("Now BEND your fingers fully and press any key to continue...");
  while (Serial.available() == 0) {}
  Serial.read();
  calibrateResistance(maxResistance);
  Serial.println("Bent position recorded.\n");
}

void calibrateResistance(float resistanceArray[5]) {
  for (int i = 0; i < 5; i++) {
    float sumResistance = 0;
    int samples = 5;
    for (int j = 0; j < samples; j++) {
      sumResistance += readFlexSensorResistance(flexSensorPins[i]);
      delay(50);
    }
    resistanceArray[i] = sumResistance / samples;
  }
}

SensorData readSensors() {
  SensorData data;
  xyzFloat acc = mpu6500.getGValues();
  float ax = acc.x - ax_offset;
  float ay = acc.y - ay_offset;
  float az = acc.z - az_offset;
  data.roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / M_PI;
  data.pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;

  for (int i = 0; i < 5; i++) {
    float currentResistance = readFlexSensorResistance(flexSensorPins[i]);
    float normalizedAngle = map(currentResistance, minResistance[i], maxResistance[i], 0, 180);
    normalizedAngle = constrain(normalizedAngle, 0, 180);
    data.angles[i] = normalizedAngle;
    data.bendStates[i] = (normalizedAngle < 45) ? "S" : "B";
  }

  // Read touch sensors
  data.touchStates[0] = digitalRead(finger1Pin) == LOW ? 1 : 0;
  data.touchStates[1] = digitalRead(finger2Pin) == LOW ? 1 : 0;
  data.touchStates[2] = digitalRead(finger3Pin) == LOW ? 1 : 0;
  data.touchStates[3] = digitalRead(finger4Pin) == LOW ? 1 : 0;

  return data;
}

void printSensorData(SensorData data) {
  Serial.print("Gyro: Pitch=");
  Serial.print(data.pitch);
  Serial.print(", Roll=");
  Serial.println(data.roll);
  
  Serial.print("Flex: Angles=");
  for (int i = 0; i < 5; i++) {
    Serial.print(data.angles[i], 1);
    if (i < 4) Serial.print(",");
  }
  Serial.print(" | Bend=");
  for (int i = 0; i < 5; i++) {
    Serial.print(data.bendStates[i]);
    if (i < 4) Serial.print(",");
  }
  Serial.println();
  
  // Serial.println(data.touchStates);
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
