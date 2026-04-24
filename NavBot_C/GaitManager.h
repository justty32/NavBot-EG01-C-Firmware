#ifndef GAITMANAGER_H
#define GAITMANAGER_H

#include <Arduino.h>
#include <math.h>
#include "Config.h"

struct GaitResult {
    float x[4];
    float y[4];
};

class GaitManager {
public:
    GaitManager() : t(0.0) {}
    
    // 計算對角步 (Trot)
    GaitResult calculateTrot(float speed, float h, float L_val, float R_val);
    
    // 計算靜步 (Walk)
    GaitResult calculateWalk(float speed, float h, float L_val, float R_val, float cg_x, float cg_y, float l_body);
    
    // 計算步態週期重置與推進
    void updateTime(float speed_increment, float ts_limit);
    
    void resetTime() { t = 0.0; }
    float getTime() const { return t; }

private:
    float t; // 當前步態相位時間
};

#endif
