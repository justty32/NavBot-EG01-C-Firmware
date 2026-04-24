#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <Arduino.h>
#include <math.h>
#include "Config.h"

struct LegAngles {
    float ham;
    float shank;
};

struct QuadAngles {
    LegAngles leg[4];
};

class Kinematics {
public:
    // ma_case: 0=Series, 1=Parallel
    static QuadAngles calculateIK(int ma_case, float l1, float l2, 
                                 float x[4], float y[4]);
                                 
private:
    static float clamp(float val, float min_v, float max_v);
};

#endif
