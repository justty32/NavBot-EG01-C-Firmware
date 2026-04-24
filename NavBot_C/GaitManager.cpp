#include "GaitManager.h"

void GaitManager::updateTime(float speed_increment, float ts_limit) {
    t += speed_increment;
    if (t >= ts_limit) {
        t = 0.0;
    }
}

GaitResult GaitManager::calculateTrot(float speed, float h, float L_val, float R_val) {
    GaitResult res;
    
    // r1..r4 轉向因子計算 (對應 padog.py:220)
    // r1=L*R, r4=L*(2-R), r2=(2-L)*R, r3=(2-L)*(2-R)
    float r1 = L_val * R_val;
    float r4 = L_val * (2.0 - R_val);
    float r2 = (2.0 - L_val) * R_val;
    float r3 = (2.0 - L_val) * (2.0 - R_val);
    
    float xf = speed; // 目標步幅
    float xs = 0.0;   // 初始 x
    
    // 對角腿相位 0.5 (FAAI)
    if (t <= TS_CYCLE * FAAI) {
        // 第一相位: 腿1, 腿3 擺動; 腿2, 腿4 支撐
        float sigma = 2.0 * M_PI * t / (FAAI * TS_CYCLE);
        float zep = h * (1.0 - cos(sigma)) / 2.0;
        float xep_b = (xf - xs) * ((sigma - sin(sigma)) / (2.0 * M_PI)) + xs;
        float xep_z = (xs - xf) * ((sigma - sin(sigma)) / (2.0 * M_PI)) + xf;
        
        // 腿1擺動
        res.x[0] = xep_z * r1; res.y[0] = zep;
        // 腿3擺動
        res.x[2] = xep_z * r3; res.y[2] = zep;
        // 腿2支撐
        res.x[1] = xep_b * r2; res.y[1] = 0.0;
        // 腿4支撐
        res.x[3] = xep_b * r4; res.y[3] = 0.0;
    } else {
        // 第二相位: 腿2, 腿4 擺動; 腿1, 腿3 支撐
        float sigma = 2.0 * M_PI * (t - FAAI * TS_CYCLE) / (FAAI * TS_CYCLE);
        float zep = h * (1.0 - cos(sigma)) / 2.0;
        float xep_b = (xf - xs) * ((sigma - sin(sigma)) / (2.0 * M_PI)) + xs;
        float xep_z = (xs - xf) * ((sigma - sin(sigma)) / (2.0 * M_PI)) + xf;
        
        // 腿2擺動
        res.x[1] = xep_z * r2; res.y[1] = zep;
        // 腿4擺動
        res.x[3] = xep_z * r4; res.y[3] = zep;
        // 腿1支撐
        res.x[0] = xep_b * r1; res.y[0] = 0.0;
        // 腿3支撐
        res.x[2] = xep_b * r3; res.y[2] = 0.0;
    }
    
    return res;
}
