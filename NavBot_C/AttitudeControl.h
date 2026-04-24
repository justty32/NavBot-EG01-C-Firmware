#ifndef ATTITUDECONTROL_H
#define ATTITUDECONTROL_H

#include <Arduino.h>
#include <math.h>
#include "Config.h"

struct AttitudeOffset {
    float x[4];
    float z[4];
};

class AttitudeControl {
public:
    static AttitudeOffset calculate(float PIT, float ROL, float l, float b, float w, float x_shift, float Hc);
};

#endif
