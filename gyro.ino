#include <Wire.h>

#define MPU6500_ADDR 0x68 // I2C address of MPU6500

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Wake up the MPU6500
    Wire.beginTransmission(MPU6500_ADDR);
    Wire.write(0x6B); // Power Management 1
    Wire.write(0x00); // Wake up the sensor
    Wire.endTransmission(true);

    Serial.println("MPU6500 Initialized");
}

void loop() {
    int16_t accelX, accelY, accelZ, gyroX, gyroY, gyroZ;

    Wire.beginTransmission(MPU6500_ADDR);
    Wire.write(0x3B); // Starting register for accelerometer data
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6500_ADDR, 14, true); // Request 14 bytes

    // Read Accelerometer data
    accelX = (Wire.read() << 8) | Wire.read();
    accelY = (Wire.read() << 8) | Wire.read();
    accelZ = (Wire.read() << 8) | Wire.read();
    Wire.read(); Wire.read(); // Ignore Temperature

    // Read Gyroscope data
    gyroX = (Wire.read() << 8) | Wire.read();
    gyroY = (Wire.read() << 8) | Wire.read();
    gyroZ = (Wire.read() << 8) | Wire.read();

    // Print data
    Serial.print("Accel X: "); Serial.print(accelX);
    Serial.print(" | Accel Y: "); Serial.print(accelY);
    Serial.print(" | Accel Z: "); Serial.println(accelZ);

    Serial.print("Gyro X: "); Serial.print(gyroX);
    Serial.print(" | Gyro Y: "); Serial.print(gyroY);
    Serial.print(" | Gyro Z: "); Serial.println(gyroZ);

    delay(500);
}
