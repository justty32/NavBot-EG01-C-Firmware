#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// 1. 機器人幾何機構參數 (單位: mm)
// ==========================================
const float L1 = 80.0;
const float L2 = 69.0;
const float BODY_L = 142.0;
const float BODY_B = 92.80001; // 嚴格對齊 Python 版精度
const float BODY_W = 108.0;

// ==========================================
// 2. 舵機中點校準與配置
// ==========================================
struct ServoConfig {
    float h_mid[4] = {90.0, 90.0, 90.0, 90.0};
    float s_mid[4] = {90.0, 90.0, 90.0, 90.0};
    int ma_case = 0;   // 0: Series, 1: Parallel
    int vmc_mode = 0;  // 0: IK, 1: VMC
    
    // 初始姿態偏置 (對應 in_pit, in_rol, in_y)
    float in_pit = 0.0;
    float in_rol = 0.0;
    float in_y = 0.0;
};

// ==========================================
// 3. 步態參數
// ==========================================
const float DEFAULT_TROT_SPEED = 0.035;
const float DEFAULT_WALK_SPEED = 0.015;
const float DEFAULT_STEP_HEIGHT = 45.0;
const float TS_CYCLE = 1.0;
const float FAAI = 0.5;

// ==========================================
// 4. 控制增益
// ==========================================
const float KP_HEIGHT = 0.06;
const float KP_ATTITUDE = 0.03;

// ==========================================
// 5. 接腳定義
// ==========================================
#define I2C_SDA_SERVO 19
#define I2C_SCL_SERVO 23
#define I2C_SDA_IMU   33
#define I2C_SCL_IMU   32

#endif
