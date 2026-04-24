#ifndef IMUSENSOR_H
#define IMUSENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "Config.h"

struct Attitude {
    float pitch;
    float roll;
    float gyroP;
    float gyroR;
};

class IMUSensor {
public:
    IMUSensor() : q0(1.0f), q1(0.0f), q2(0.0f), q3(0.0f), integralFBx(0.0f), integralFBy(0.0f), integralFBz(0.0f) {}
    
    bool init();
    void update();
    Attitude getAttitude();

private:
    // MPU6050 暫存器
    const uint8_t MPU6050_ADDR = 0x68;
    const uint8_t PWR_MGMT_1 = 0x6B;
    const uint8_t ACCEL_XOUT_H = 0x3B;
    const uint8_t GYRO_XOUT_H = 0x43;

    // Mahony 濾波器參數 (對應 PA_IMU.py)
    const float Kp = 0.8f;
    const float Ki = 0.001f;
    const float halfT = 0.005f; // 10ms loop -> halfT = 0.005
    const float K_blend = 0.7f; // 混合權重

    // 四元數與積分項
    float q0, q1, q2, q3;
    float integralFBx, integralFBy, integralFBz;
    
    // 零點校準
    float gyroBiasX = 0, gyroBiasY = 0, gyroBiasZ = 0;

    Attitude currentAttitude;
    
    void readRaw(int16_t* acc, int16_t* gyro);
    void mahonyUpdate(float gx, float gy, float gz, float ax, float ay, float az);
};

#endif
