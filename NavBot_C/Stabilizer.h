#ifndef STABILIZER_H
#define STABILIZER_H

#include <Arduino.h>
#include <deque>
#include "IMUSensor.h"

class Stabilizer {
public:
    Stabilizer() : Sta_Pitch(0), Sta_Roll(0), recover_node(0), recover_node_1(0) {}
    
    void update(Attitude att, float& targetP, float& targetR);
    int checkFall(Attitude att); // 0: OK, 1: Fall Left, 2: Fall Right

private:
    float Sta_Pitch;
    float Sta_Roll;
    float recover_node;
    float recover_node_1;

    // 移動平均緩衝 (C++ 實作)
    std::deque<float> filter_buf_p;
    std::deque<float> filter_buf_r;
    
    float getAvg(std::deque<float>& buf, float newValue, int windowSize);
    
    const float kp_sta = 0.1f;
    const float deadzone = 3.0f;
    const float max_ang = 25.0f;
};

#endif
