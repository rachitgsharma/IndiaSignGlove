#include <Wire.h>
#include <MPU6500_WE.h>
#include <math.h>

#define MPU6500_ADDR 0x68  // I2C Address of MPU6500

MPU6500_WE mpu6500 = MPU6500_WE(MPU6500_ADDR);

float ax_offset = 0, ay_offset = 0, az_offset = 0;
float gx_offset = 0, gy_offset = 0, gz_offset = 0;

void calibrateMPU() {
    Serial.println("Calibrating MPU6500... Hold still.");
    
    int samples = 100;
    for (int i = 0; i < samples; i++) {
        xyzFloat acc = mpu6500.getGValues();
        xyzFloat gyro = mpu6500.getGyrValues();
        
        ax_offset += acc.x;
        ay_offset += acc.y;
        az_offset += acc.z - 1.0;  // Adjust for gravity
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

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!mpu6500.init()) {
        Serial.println("MPU6500 Connection Failed!");
        while (1);
    }

    Serial.println("MPU6500 Initialized!");
    mpu6500.enableGyrDLPF();
    mpu6500.setGyrDLPF(5);
    mpu6500.setGyrRange(MPU6500_GYRO_RANGE_250);
    mpu6500.setAccRange(MPU6500_ACC_RANGE_4G);

    calibrateMPU();
}

void loop() {
    xyzFloat acc = mpu6500.getGValues();
    
    // Apply Calibration
    float ax = acc.x - ax_offset;
    float ay = acc.y - ay_offset;
    float az = acc.z - az_offset;

    // Compute Pitch & Roll
    int roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / M_PI;
    int pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;

    // Send Pitch & Roll over Serial
    Serial.print(pitch); Serial.print(",");
    Serial.println(roll);

    delay(100);
}