#include <Wire.h>
#include <MPU6500_WE.h>

#define MPU6500_ADDR 0x68  
MPU6500_WE mpu = MPU6500_WE(MPU6500_ADDR);

// Flex Sensor Configuration
const int flexSensorPins[5] = {A0, A1, A2, A3, A4};  
const float supplyVoltage = 5.0;
const int pullUpResistor = 15000;  
float baselineResistance[5];  
const float resistanceToAngleFactor = 0.05;  
const float STRAIGHT_THRESHOLD = 45.0;
const float PARTIALLY_BENT_THRESHOLD = 180.0;

// Touch Sensor Configuration
const int touchPins[5] = {2, 3, 4, 5, 6};  
int touchStates[5] = {0};  

// Gyroscope & Accelerometer Variables
float roll = 0, pitch = 0;
float gyroRoll = 0, gyroPitch = 0;
float gyroBiasX = 0, gyroBiasY = 0;
unsigned long lastTime;

// **Calibration Function for Gyroscope**
void calibrateMPU6500() {
    Serial.println("Calibrating MPU6500...");
    float sumX = 0, sumY = 0;
    int samples = 200;  

    for (int i = 0; i < samples; i++) {
        xyzFloat gyr = mpu.getGyrValues();
        sumX += gyr.x;
        sumY += gyr.y;
        delay(10);  
    }

    gyroBiasX = sumX / samples;
    gyroBiasY = sumY / samples;
    Serial.println("Calibration Complete!");
}

// **Setup Function**
void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize MPU6500
    // if (!mpu.init()) {
    //     Serial.println("MPU6500 not detected!");
    //     while (1);
    // }

    // mpu.enableGyrDLPF();
    // mpu.setGyrDLPF(MPU6500_DLPF_6);
    // mpu.setAccDLPF(MPU6500_DLPF_6);

    // Serial.println("MPU6500 Initialized Successfully!");
    // calibrateMPU6500();  
    lastTime = millis();

    // **Calibrate Flex Sensors**
    Serial.println("Calibrating Flex Sensors...");
    for (int i = 0; i < 5; i++) {
        float sumResistance = 0;
        int samples = 10;  

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

    // **Initialize Touch Sensors**
    for (int i = 0; i < 5; i++) {
        pinMode(touchPins[i], INPUT);
    }

    delay(2000);
}

// **Main Loop**
void loop() {
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0;
    lastTime = currentTime;

    // **Read Flex Sensor Angles**
    float angles[5];
    String bendStates[5];

    for (int i = 0; i < 5; i++) {
        float currentResistance = readFlexSensorResistance(flexSensorPins[i]);
        float deltaResistance = currentResistance - baselineResistance[i];
        angles[i] = abs(deltaResistance) * resistanceToAngleFactor;

        // Determine bending position
        if (angles[i] < STRAIGHT_THRESHOLD) {
            bendStates[i] = "S";  
        } else if (angles[i] < PARTIALLY_BENT_THRESHOLD) {
            bendStates[i] = "P";  
        } else {
            bendStates[i] = "B";  
        }
    }

    // **Read Touch Sensor States**
    for (int i = 0; i < 5; i++) {
        touchStates[i] = digitalRead(touchPins[i]);
    }

    // **Read Gyroscope & Accelerometer Data**
    xyzFloat acc = mpu.getGValues();
    xyzFloat gyr = mpu.getGyrValues();

    // Remove Bias from Gyro Readings
    gyr.x -= gyroBiasX;
    gyr.y -= gyroBiasY;

    // Compute roll & pitch from accelerometer
    float accRoll = atan2(acc.y, acc.z) * 57.2958;
    float accPitch = atan2(-acc.x, sqrt(acc.y * acc.y + acc.z * acc.z)) * 57.2958;

    // Integrate gyroscope data for roll & pitch
    gyroRoll += gyr.x * dt;
    gyroPitch += gyr.y * dt;

    // Complementary Filter: 98% gyro + 2% accelerometer
    roll = 0.98 * gyroRoll + 0.02 * accRoll;
    pitch = 0.98 * gyroPitch + 0.02 * accPitch;

    // **Output Data in JSON Format**
    Serial.print("{");
    
    Serial.print("\"angles\": [");
    for (int i = 0; i < 5; i++) {
        Serial.print(angles[i], 1);
        if (i < 4) Serial.print(", ");
    }
    Serial.print("], ");

    Serial.print("\"bends\": [");
    for (int i = 0; i < 5; i++) {
        Serial.print("\"");
        Serial.print(bendStates[i]);
        Serial.print("\"");
        if (i < 4) Serial.print(", ");
    }
    Serial.print("], ");

    Serial.print("\"touch\": [");
    for (int i = 0; i < 5; i++) {
        Serial.print(touchStates[i]);
        if (i < 4) Serial.print(", ");
    }
    Serial.print("], ");

    Serial.print("\"gyro\": {\"roll\": ");
    Serial.print(roll, 2);
    Serial.print(", \"pitch\": ");
    Serial.print(pitch, 2);
    Serial.print("}}");

    Serial.println();

    delay(100);  
}

// **Function to Read Flex Sensor Resistance**
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
