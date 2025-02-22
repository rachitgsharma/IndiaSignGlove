#include <Wire.h>
#include <MPU6500_WE.h>

#define MPU6500_ADDR 0x68  // I2C Address
MPU6500_WE mpu = MPU6500_WE(MPU6500_ADDR);

// Variables
float roll = 0, pitch = 0;
float gyroRoll = 0, gyroPitch = 0;
float gyroBiasX = 0, gyroBiasY = 0;
unsigned long lastTime;

// **Calibration Function**
void calibrateMPU6500() {
    Serial.println("Calibrating MPU6500...");
    float sumX = 0, sumY = 0;
    int samples = 200;  // Number of readings for calibration

    for (int i = 0; i < samples; i++) {
        xyzFloat gyr = mpu.getGyrValues();
        sumX += gyr.x;
        sumY += gyr.y;
        delay(10);  // Short delay for stable readings
    }

    // Compute Bias (Average of samples)
    gyroBiasX = sumX / samples;
    gyroBiasY = sumY / samples;
    Serial.println("Calibration Complete!");
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize MPU6500
    if (!mpu.init()) {
        Serial.println("MPU6500 not detected!");
        while (1);
    }

    // Configure Sensor Settings
    mpu.enableGyrDLPF();
    mpu.setGyrDLPF(MPU6500_DLPF_6);
    mpu.setAccDLPF(MPU6500_DLPF_6);

    Serial.println("MPU6500 Initialized Successfully!");

    // **Calibrate the gyroscope**
    calibrateMPU6500();
    
    lastTime = millis();
}

void loop() {
    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0;  // Time step in seconds
    lastTime = currentTime;

    // Read accelerometer and gyroscope data
    xyzFloat acc = mpu.getGValues();
    xyzFloat gyr = mpu.getGyrValues();

    // **Remove Bias from Gyro Readings**
    gyr.x -= gyroBiasX;
    gyr.y -= gyroBiasY;

    // Compute roll & pitch from accelerometer
    float accRoll = atan2(acc.y, acc.z) * 57.2958;
    float accPitch = atan2(-acc.x, sqrt(acc.y * acc.y + acc.z * acc.z)) * 57.2958;

    // Integrate gyroscope data for roll & pitch
    gyroRoll += gyr.x * dt;
    gyroPitch += gyr.y * dt;

    // Complementary filter: 98% gyro + 2% accelerometer
    roll = 0.98 * gyroRoll + 0.02 * accRoll;
    pitch = 0.98 * gyroPitch + 0.02 * accPitch;

    // Print results
    Serial.print("Roll: "); Serial.print(roll); Serial.print("°  ");
    Serial.print("Pitch: "); Serial.print(pitch); Serial.println("°");
    Serial.println("------------------------");

    delay(100);  // 10Hz sampling rate
}
