#include "AttitudeControl.h"

AttitudeOffset AttitudeControl::calculate(float PIT, float ROL, float l, float b, float w, float x_shift, float Hc) {
    AttitudeOffset res;
    float P = PIT * PI / 180.0f;
    float R = ROL * PI / 180.0f;
    float Y = 0.0f; // Yaw 固定為 0

    float cosP = cos(P); float sinP = sin(P);
    float cosR = cos(R); float sinR = sin(R);
    
    // 嚴格復刻 PA_ATTITUDE.py 的旋轉矩陣展開式
    // 腿 1 (前左)
    res.x[0] = l/2.0f - x_shift - (l*cosP)/2.0f;
    res.z[0] = -Hc - (b*sinR)/2.0f - (l*cosR*sinP)/2.0f;

    // 腿 2 (前右)
    res.x[1] = l/2.0f - x_shift - (l*cosP)/2.0f;
    res.z[1] = -Hc + (b*sinR)/2.0f - (l*cosR*sinP)/2.0f;

    // 腿 3 (後左) -> 映射自 Python 版 AB4 (後右邏輯)
    // 注意：Python 版 x3 = AB4, y3 = AB4_z
    res.x[2] = -l/2.0f - x_shift + (l*cosP)/2.0f;
    res.z[2] = -Hc + (b*sinR)/2.0f + (l*cosR*sinP)/2.0f;

    // 腿 4 (後右) -> 映射自 Python 版 AB3 (後左邏輯)
    // 注意：Python 版 x4 = AB3, y4 = AB3_z
    res.x[3] = -l/2.0f - x_shift + (l*cosP)/2.0f;
    res.z[3] = -Hc - (b*sinR)/2.0f + (l*cosR*sinP)/2.0f;

    return res;
}
