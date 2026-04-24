#include "Kinematics.h"

float Kinematics::clamp(float val, float min_v, float max_v) {
    if (val < min_v) return min_v;
    if (val > max_v) return max_v;
    return val;
}

QuadAngles Kinematics::calculateIK(int ma_case, float l1, float l2, 
                                 float x_in[4], float y_in[4]) {
    QuadAngles res;
    
    if (ma_case == 0) { // ============= Series Legs =============
        for (int i = 0; i < 4; i++) {
            float x = -x_in[i];  // 同 Python 版的符號反轉
            float y = y_in[i];
            float L_sq = x*x + y*y;
            float L = sqrt(L_sq);
            if (L < 0.001) L = 0.001; // 防止除以零
            
            // 1. 求小腿角度 (Shank)
            float cos_shank = (L_sq - l1*l1 - l2*l2) / (-2.0 * l1 * l2);
            cos_shank = clamp(cos_shank, -1.0, 1.0); // 保護機制
            float shank_rad = PI - acos(cos_shank);
            
            // 2. 求大腿幾何偏移角 fai
            float cos_fai = (l1*l1 + L_sq - l2*l2) / (2.0 * l1 * L);
            cos_fai = clamp(cos_fai, -1.0, 1.0);
            float fai = acos(cos_fai);
            
            // 3. 求大腿與垂直軸夾角
            float ham_rad;
            if (fabs(x) > 0.001) {
                if (x > 0) {
                    ham_rad = fabs(atan(y / x)) - fai;
                } else {
                    ham_rad = PI - fabs(atan(y / x)) - fai;
                }
            } else {
                ham_rad = PI - 1.570796 - fai; // 1.570796 = pi/2
            }
            
            res.leg[i].shank = 180.0 * shank_rad / PI;
            res.leg[i].ham = 180.0 * ham_rad / PI;
        }
    } else { // ============= Parallel Legs =============
        for (int i = 0; i < 4; i++) {
            float x = x_in[i];
            float y = -y_in[i]; // 並聯腿 y 方向取反
            float L = sqrt(x*x + y*y);
            if (L < 0.001) L = 0.001;
            
            float psai = asin(x / L);
            float cos_fai = (L*L + l1*l1 - l2*l2) / (2.0 * l1 * L);
            cos_fai = clamp(cos_fai, -1.0, 1.0);
            float fai = acos(cos_fai);
            
            res.leg[i].ham = 180.0 * (fai - psai) / PI;   // sita1
            res.leg[i].shank = 180.0 * (fai + psai) / PI; // sita2
        }
    }
    
    return res;
}
