#include "IMUSensor.h"

bool IMUSensor::init() {
    Wire1.begin(I2C_SDA_IMU, I2C_SCL_IMU);

    // 喚醒 MPU6050
    Wire1.beginTransmission(MPU6050_ADDR);
    Wire1.write(PWR_MGMT_1);
    Wire1.write(0);
    if (Wire1.endTransmission() != 0) return false;

    // 進行簡單的陀螺儀靜止校準 (300 樣本)
    Serial.println("Calibrating IMU, keep robot still...");
    float sx = 0, sy = 0, sz = 0;
    for (int i = 0; i < 300; i++) {
        int16_t acc[3], gyro[3];
        readRaw(acc, gyro);
        sx += gyro[0]; sy += gyro[1]; sz += gyro[2];
        delay(2);
    }
    gyroBiasX = sx / 300.0f;
    gyroBiasY = sy / 300.0f;
    gyroBiasZ = sz / 300.0f;
    Serial.println("IMU Calibrated.");
    
    return true;
}

void IMUSensor::readRaw(int16_t* acc, int16_t* gyro) {
    Wire1.beginTransmission(MPU6050_ADDR);
    Wire1.write(ACCEL_XOUT_H);
    Wire1.endTransmission(false);
    Wire1.requestFrom(MPU6050_ADDR, (uint8_t)14);

    acc[0] = (Wire1.read() << 8) | Wire1.read();
    acc[1] = (Wire1.read() << 8) | Wire1.read();
    acc[2] = (Wire1.read() << 8) | Wire1.read();
    Wire1.read(); Wire1.read(); // Skip Temp
    gyro[0] = (Wire1.read() << 8) | Wire1.read();
    gyro[1] = (Wire1.read() << 8) | Wire1.read();
    gyro[2] = (Wire1.read() << 8) | Wire1.read();
}

void IMUSensor::mahonyUpdate(float gx, float gy, float gz, float ax, float ay, float az) {
    float recipNorm;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float qa, qb, qc;

    // 正規化加速度
    float norm = sqrt(ax * ax + ay * ay + az * az);
    if (norm == 0.0f) return;
    recipNorm = 1.0f / norm;
    ax *= recipNorm; ay *= recipNorm; az *= recipNorm;

    // 估計重力方向 (由四元數推算)
    halfvx = q1 * q3 - q0 * q2;
    halfvy = q0 * q1 + q2 * q3;
    halfvz = q0 * q0 - 0.5f + q3 * q3;

    // 誤差為估計重力與測量重力的叉積
    halfex = (ay * halfvz - az * halfvy);
    halfey = (az * halfvx - ax * halfvz);
    halfez = (ax * halfvy - ay * halfvx);

    // PI 修正
    if (Ki > 0.0f) {
        integralFBx += Ki * halfex * (2.0f * halfT);
        integralFBy += Ki * halfey * (2.0f * halfT);
        integralFBz += Ki * halfez * (2.0f * halfT);
        gx += integralFBx; gy += integralFBy; gz += integralFBz;
    }
    gx += Kp * halfex; gy += Kp * halfey; gz += Kp * halfez;

    // 積分更新四元數
    gx *= halfT; gy *= halfT; gz *= halfT;
    qa = q0; qb = q1; qc = q2;
    q0 += (-qb * gx - qc * gy - q3 * gz);
    q1 += (qa * gx + qc * gz - q3 * gy);
    q2 += (qa * gy - qb * gz + q3 * gx);
    q3 += (qa * gz + qb * gy - qc * gx);

    // 正規化四元數
    recipNorm = 1.0f / sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm; q1 *= recipNorm; q2 *= recipNorm; q3 *= recipNorm;
}

void IMUSensor::update() {
    int16_t rawAcc[3], rawGyro[3];
    readRaw(rawAcc, rawGyro);

    // 單位換算
    float gx = (rawGyro[0] - gyroBiasX) / 65.5f * (PI / 180.0f);
    float gy = (rawGyro[1] - gyroBiasY) / 65.5f * (PI / 180.0f);
    float gz = (rawGyro[2] - gyroBiasZ) / 65.5f * (PI / 180.0f);
    float ax = (float)rawAcc[0];
    float ay = (float)rawAcc[1];
    float az = (float)rawAcc[2];

    mahonyUpdate(gx, gy, gz, ax, ay, az);

    // 轉換為歐拉角 (度) - 嚴格對應 PA_IMU.py:53
    // Pitch = math.asin(2 * q0 * q2 - 2 * q1 * q3) * 57.3
    // Roll = math.atan2(2 * q1 * q2 + 2 * q0 * q3, q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3
    float pitch_mahony = asin(2.0f * (q0 * q2 - q1 * q3)) * 180.0f / PI;
    float roll_mahony = atan2(2.0f * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 180.0f / PI;

    // 純加速度計估計 (輔助穩定)
    float pitch_acc = atan2(ay, az) * 180.0f / PI;
    float roll_acc = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0f / PI;

    // 二次融合 (對應 PA_IMU.py:58)
    currentAttitude.pitch = -(K_blend * pitch_mahony + (1.0f - K_blend) * pitch_acc);
    currentAttitude.roll = K_blend * roll_mahony + (1.0f - K_blend) * roll_acc;
    currentAttitude.gyroP = gx * 180.0f / PI;
    currentAttitude.gyroR = gy * 180.0f / PI;
}

Attitude IMUSensor::getAttitude() {
    return currentAttitude;
}
