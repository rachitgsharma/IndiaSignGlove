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
float baselineResistance[5];
const float resistanceToAngleFactor = 0.05;
const float STRAIGHT_THRESHOLD = 45.0;
const float PARTIALLY_BENT_THRESHOLD = 180.0;

// Touch Sensors
const int finger1Pin = 2;
const int finger2Pin = 3;
const int finger3Pin = 4;
const int finger4Pin = 5;

// Data Structures
struct GyroData {
  int pitch;
  int roll;
};

struct FlexSensorData {
  float angles[5];
  String bendStates[5];
};

// Timing
unsigned long previousMillisGyro = 0;
const long intervalGyro = 100; // 100ms
unsigned long previousMillisFlex = 0;
const long intervalFlex = 500; // 500ms
unsigned long previousMillisTouch = 0;
const long intervalTouch = 50; // 50ms

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Gyro Setup
  if (!mpu6500.init()) {
    Serial.println("MPU6500 Connection Failed!");
    while (1);
  }
  mpu6500.enableGyrDLPF();
  mpu6500.setGyrDLPF(5);
  mpu6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  mpu6500.setAccRange(MPU6500_ACC_RANGE_4G);
  calibrateMPU();

  // Flex Sensor Setup
  Serial.println("Calibrating Flex Sensors...");
  calibrateFlexSensors();
  Serial.println("Calibration Complete.\n");

  // Touch Sensor Setup
  pinMode(finger1Pin, INPUT_PULLUP);
  pinMode(finger2Pin, INPUT_PULLUP);
  pinMode(finger3Pin, INPUT_PULLUP);
  pinMode(finger4Pin, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillisGyro >= intervalGyro) {
    previousMillisGyro = currentMillis;
    GyroData gyroData = readGyro();
    printGyroData(gyroData);
  }

  if (currentMillis - previousMillisFlex >= intervalFlex) {
    previousMillisFlex = currentMillis;
    FlexSensorData flexData = readFlexSensors();
    printFlexSensorData(flexData);
  }

  if (currentMillis - previousMillisTouch >= intervalTouch) {
    previousMillisTouch = currentMillis;
    readTouchSensors();
    Serial.println(); // Add a newline after touch data
  }
}

// Gyro Functions
void calibrateMPU() {
  Serial.println("Calibrating MPU6500... Hold still.");
  int samples = 100;
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

GyroData readGyro() {
  xyzFloat acc = mpu6500.getGValues();
  float ax = acc.x - ax_offset;
  float ay = acc.y - ay_offset;
  float az = acc.z - az_offset;
  GyroData data;
  data.roll = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / M_PI;
  data.pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
  return data;
}

void printGyroData(GyroData data) {
  Serial.print("Gyro: Pitch=");
  Serial.print(data.pitch);
  Serial.print(", Roll=");
  Serial.println(data.roll);
}

// Flex Sensor Functions
void calibrateFlexSensors() {
  for (int i = 0; i < 5; i++) {
    float sumResistance = 0;
    int samples = 5;
    for (int j = 0; j < samples; j++) {
      sumResistance += readFlexSensorResistance(flexSensorPins[i]);
      delay(50);
    }
    baselineResistance[i] = sumResistance / samples;
  }
}

FlexSensorData readFlexSensors() {
  FlexSensorData data;
  for (int i = 0; i < 5; i++) {
    float currentResistance = readFlexSensorResistance(flexSensorPins[i]);
    float deltaResistance = currentResistance - baselineResistance[i];
    data.angles[i] = abs(deltaResistance) * resistanceToAngleFactor;
    if (data.angles[i] < STRAIGHT_THRESHOLD) {
      data.bendStates[i] = "S";
    } else if (data.angles[i] >= STRAIGHT_THRESHOLD && data.angles[i] < PARTIALLY_BENT_THRESHOLD) {
      data.bendStates[i] = "P";
    }
  }
  return data;
}

void printFlexSensorData(FlexSensorData data) {
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

// Touch Sensor Functions
void readTouchSensors() {
  bool touched = false;
  Serial.print("Touch: ");
  if (digitalRead(finger1Pin) == LOW) {
    Serial.print("1");
    touched = true;
  }
  if (digitalRead(finger2Pin) == LOW) {
    if (touched) Serial.print(",2");
    else Serial.print("2");
    touched = true;
  }
  if (digitalRead(finger3Pin) == LOW) {
    if (touched) Serial.print(",3");
    else Serial.print("3");
    touched = true;
  }
  if (digitalRead(finger4Pin) == LOW) {
    if (touched) Serial.print(",4");
    else Serial.print("4");
    touched = true;
  }
  if (!touched) {
      Serial.print("None");
  }

}