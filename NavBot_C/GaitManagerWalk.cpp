#include "GaitManager.h"
#include "AttitudeControl.h"

// 宣告全域變數 (供 Walk COG 計算使用)
extern float Y_goal;
extern float current_H;
extern float PIT_current;
extern float ROL_current;
extern float Y_current;

GaitResult GaitManager::calculateWalk(float speed, float h, float L_val, float R_val, float cg_x, float cg_y, float l_body) {
    GaitResult res;
    
    // 初始化所有腿在原地
    for (int i=0; i<4; i++) {
        res.x[i] = 0;
        res.y[i] = 0;
    }
    
    // Walk 步幅目標
    float xf = (speed * 7.0f); // 對應原版的 spd*7
    
    // 取得目前的 Pitch 角 (用於補償重心地形偏移)
    float pit_rad = PIT_current * PI / 180.0f;
    
    // 一個完整的 Walk 週期是 4 * FAAI * TS_CYCLE = 2.0 (預設)
    float phase1_end = FAAI * TS_CYCLE;
    float phase2_end = 2.0f * FAAI * TS_CYCLE;
    float phase3_end = 3.0f * FAAI * TS_CYCLE;
    float phase4_end = 4.0f * FAAI * TS_CYCLE;

    if (t <= phase1_end) {
        // --- 腿 1 (前左) 擺動 ---
        // 1. 先把重心移向右後方 (period = -1)
        float target_y_shift = cg_y + (-1.0f) * (l_body + cg_x)/4.0f + 100.0f * tan(pit_rad);
        Y_goal = target_y_shift;
        
        // 2. 檢查重心是否到位 (Y_current 是實際的平移量)
        if (fabs(Y_current - target_y_shift) < 3.0f) {
            updateTime(DEFAULT_WALK_SPEED, phase4_end);
        }

        // 3. 軌跡計算 (前半週期)
        float sigma = 2.0f * M_PI * t / phase1_end;
        res.y[0] = h * (1.0f - cos(sigma)) / 2.0f;
        res.x[0] = xf * ((sigma - sin(sigma)) / (2.0f * M_PI));

    } else if (t > phase1_end && t <= phase2_end) {
        // --- 腿 2 (前右) 擺動 ---
        float target_y_shift = -cg_y + (-1.0f) * (l_body + cg_x)/4.0f + 100.0f * tan(pit_rad);
        Y_goal = target_y_shift;
        
        if (fabs(Y_current - target_y_shift) < 3.0f) {
            updateTime(DEFAULT_WALK_SPEED, phase4_end);
        }

        float sigma = 2.0f * M_PI * (t - phase1_end) / phase1_end;
        res.y[1] = h * (1.0f - cos(sigma)) / 2.0f;
        res.x[1] = xf * ((sigma - sin(sigma)) / (2.0f * M_PI));
        res.x[0] = xf;

    } else if (t > phase2_end && t <= phase3_end) {
        // --- 腿 3 (後左) 擺動 ---
        float target_y_shift = cg_y + (1.0f) * (l_body + cg_x)/4.0f + 100.0f * tan(pit_rad);
        Y_goal = target_y_shift;
        
        if (fabs(Y_current - target_y_shift) < 3.0f) {
            updateTime(DEFAULT_WALK_SPEED, phase4_end);
        }

        float sigma = 2.0f * M_PI * (t - phase2_end) / phase1_end;
        res.y[2] = h * (1.0f - cos(sigma)) / 2.0f;
        res.x[2] = xf * ((sigma - sin(sigma)) / (2.0f * M_PI));
        res.x[0] = xf;
        res.x[1] = xf;

    } else if (t > phase3_end && t <= phase4_end) {
        // --- 腿 4 (後右) 擺動 ---
        float target_y_shift = -cg_y + (1.0f) * (l_body + cg_x)/4.0f + 100.0f * tan(pit_rad);
        Y_goal = target_y_shift;
        
        if (fabs(Y_current - target_y_shift) < 3.0f) {
            updateTime(DEFAULT_WALK_SPEED, phase4_end);
        }

        float sigma = 2.0f * M_PI * (t - phase3_end) / phase1_end;
        res.y[3] = h * (1.0f - cos(sigma)) / 2.0f;
        res.x[3] = xf * ((sigma - sin(sigma)) / (2.0f * M_PI));
        res.x[0] = xf;
        res.x[1] = xf;
        res.x[2] = xf;
        
        if (t >= phase4_end - 0.02f) {
            t = 0.0f; 
        }
    }

    return res;
}
