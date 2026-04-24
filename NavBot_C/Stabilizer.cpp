#include "Stabilizer.h"

float Stabilizer::getAvg(std::deque<float>& buf, float newValue, int windowSize) {
    buf.push_back(newValue);
    if (buf.size() > windowSize) {
        buf.pop_front();
    }
    float sum = 0;
    for (float v : buf) sum += v;
    return sum / buf.size();
}

void Stabilizer::update(Attitude att, float& targetP, float& targetR) {
    // 最終修正：嚴格對齊 PA_STABLIZE.py
    // Python 版 kp_sta = 0.1 直接作用在度數上，不需 LSB 補償
    const float C_KP_STA = 0.1f; 

    // 1. 濾波 (復刻軸交叉：Roll 角度驅動 Pitch 修正)
    float filP = getAvg(filter_buf_p, att.roll, 27); 
    float filR = getAvg(filter_buf_r, att.pitch, 47);

    // 2. Pitch 閉迴路
    if (att.roll > deadzone || att.roll < -deadzone) {
        Sta_Pitch -= filP * C_KP_STA;
    }
    Sta_Pitch = constrain(Sta_Pitch, -max_ang, max_ang);

    // 3. Roll 閉迴路
    if (att.pitch > deadzone || att.pitch < -deadzone) {
        Sta_Roll -= filR * C_KP_STA;
    }
    Sta_Roll = constrain(Sta_Roll, -max_ang, max_ang);

    // 4. 輸出
    targetP = Sta_Pitch;
    targetR = Sta_Roll;
}

int Stabilizer::checkFall(Attitude att) {
    if (att.roll <= -50.0f) {
        recover_node += 0.1f;
        if (recover_node >= 6.0f) { 
            recover_node = 0;
            return 1;
        }
    } else if (att.roll >= 50.0f) {
        recover_node_1 += 0.1f;
        if (recover_node_1 >= 6.0f) {
            recover_node_1 = 0;
            return 2;
        }
    } else {
        recover_node = 0;
        recover_node_1 = 0;
    }
    return 0;
}
