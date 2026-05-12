#include <Arduino.h>
#include "Config.h"
#include "ServoDriver.h"
#include "Kinematics.h"
#include "GaitManager.h"
#include "WebServerHandler.h"
#include "IMUSensor.h"
#include "Stabilizer.h"
#include "AttitudeControl.h"

// ==========================================
// 全域物件與狀態
// ==========================================
ServoDriver servos;
GaitManager gait;
ServoConfig servo_cfg;
WebServerHandler web;
IMUSensor imu;
Stabilizer stabilizer;

// 控制目標
float spd = 0.0;
float L = 1.0;
float R = 1.0;
float H_goal = 100.0;
float current_H = 100.0;
float PIT_goal = 0.0;
float ROL_goal = 0.0;
float Y_goal = 0.0;
float PIT_current = 0.0;
float ROL_current = 0.0;
float Y_current = 0.0;

int gait_mode = 0; 
bool key_stab = false;
int init_case = 0; 

TaskHandle_t ControlTaskHandle;

// ==========================================
// 10ms 控制主迴圈 (核心 1)
// ==========================================
void ControlLoopTask(void * pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(10); 
    xLastWakeTime = xTaskGetTickCount();

    for(;;) {
        // 1. 感測與解算
        imu.update();
        Attitude currentAtt = imu.getAttitude();

        // 2. 控制律收斂
        float targetP = PIT_goal;
        float targetR = ROL_goal;

        if (fabs(spd) < 0.001 && key_stab) {
            float stabP = 0, stabR = 0;
            stabilizer.update(currentAtt, stabP, stabR);
            targetP = stabP;
            targetR = stabR;
        }

        PIT_current += (targetP - PIT_current) * KP_ATTITUDE;
        ROL_current += (targetR - ROL_current) * KP_ATTITUDE;
        Y_current   += (Y_goal - Y_current) * KP_ATTITUDE;
        current_H   += (H_goal - current_H) * KP_HEIGHT;

        PIT_current = constrain(PIT_current, -10.0f, 10.0f);
        ROL_current = constrain(ROL_current, -10.0f, 10.0f);

        // 3. 步態生成
        GaitResult gaitRes;
        if (gait_mode == 0) { // Trot
            if (fabs(spd) > 0.001 || fabs(L - 1.0) > 0.01 || fabs(R - 1.0) > 0.01) {
                gait.updateTime(DEFAULT_TROT_SPEED, TS_CYCLE);
            } else {
                gait.resetTime();
            }
            gaitRes = gait.calculateTrot(spd * 4.0, DEFAULT_STEP_HEIGHT, L, R);
        } else { // Walk
            if (fabs(spd) < 0.001 && fabs(L - 1.0) < 0.01 && fabs(R - 1.0) < 0.01) {
                gait.resetTime();
            }
            gaitRes = gait.calculateWalk(spd, 45.0, L, R, 0.0, 17.5, BODY_L);
        }

        // 4. 姿態補償
        AttitudeOffset attOff = AttitudeControl::calculate(PIT_current, ROL_current, BODY_L, BODY_B, BODY_W, Y_current, current_H);

        // 5. 座標疊加與 IK 求解
        float total_x[4], total_y[4];
        for (int i = 0; i < 4; i++) {
            total_x[i] = gaitRes.x[i] + attOff.x[i];
            total_y[i] = gaitRes.y[i] + attOff.z[i]; 
        }
        QuadAngles angles = Kinematics::calculateIK(servo_cfg.ma_case, L1, L2, total_x, total_y);

        // 6. 輸出至舵機
        if (init_case == 1) {
            for(int i=4; i<=11; i++) servos.setAngle(i, 90.0f);
        } 
        else if (servo_cfg.ma_case == 0) {
            servos.setAngle(4,  servo_cfg.h_mid[0] - angles.leg[0].ham);
            servos.setAngle(5, (servo_cfg.s_mid[0] - 90.0) + angles.leg[0].shank);
            servos.setAngle(6,  servo_cfg.h_mid[1] + angles.leg[1].ham);
            servos.setAngle(7, (servo_cfg.s_mid[1] + 90.0) - angles.leg[1].shank);
            servos.setAngle(8,  servo_cfg.h_mid[2] + angles.leg[2].ham);
            servos.setAngle(9, (servo_cfg.s_mid[2] + 90.0) - angles.leg[2].shank);
            servos.setAngle(10, servo_cfg.h_mid[3] - angles.leg[3].ham);
            servos.setAngle(11, (servo_cfg.s_mid[3] - 90.0) + angles.leg[3].shank);
        } 
        else {
            servos.setAngle(4, (servo_cfg.h_mid[0] - 90.0) + angles.leg[0].ham);
            servos.setAngle(5, (servo_cfg.s_mid[0] - 90.0) + 180.0 - angles.leg[0].shank);
            servos.setAngle(6, (servo_cfg.h_mid[1] - 90.0) + 180.0 - angles.leg[1].ham);
            servos.setAngle(7, (servo_cfg.s_mid[1] - 90.0) + angles.leg[1].shank);
            servos.setAngle(8, (servo_cfg.h_mid[2] - 90.0) + 180.0 - angles.leg[2].ham);
            servos.setAngle(9, (servo_cfg.s_mid[2] - 90.0) + angles.leg[2].shank);
            servos.setAngle(10, (servo_cfg.h_mid[3] - 90.0) + angles.leg[3].ham);
            servos.setAngle(11, (servo_cfg.s_mid[3] - 90.0) + 180.0 - angles.leg[3].shank);
        }

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000); // 啟動緩衝
    
    Serial.println("Initializing Hardware...");
    servos.init();
    
    // 安全初始化 IMU：即使報錯也不卡死
    if (!imu.init()) {
        Serial.println("Warning: IMU not found (Normal for bare board).");
    } else {
        Serial.println("IMU Initialized.");
    }
    
    // 預載入初始位移
    PIT_current = PIT_goal = servo_cfg.in_pit;
    ROL_current = ROL_goal = servo_cfg.in_rol;
    Y_current   = Y_goal   = servo_cfg.in_y;

    // 給網路層一點啟動空間，避免 Core 3.x 崩潰
    delay(1000);
    web.init();
    
    // 創建控制任務
    xTaskCreatePinnedToCore(ControlLoopTask, "ControlTask", 8192, NULL, 1, &ControlTaskHandle, 1);
    
    Serial.println("NavBot C-Version Started Successfully.");
}

void loop() {
    // 監控系統健康狀況
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 5000) {
        Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        lastPrint = millis();
    }
    delay(10);
}
