#ifndef SERVODRIVER_H
#define SERVODRIVER_H

#include <Arduino.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "Config.h"

class ServoDriver {
public:
    ServoDriver() : pwm(0x40) {}
    
    void init();
    void setAngle(uint8_t channel, float angle);
    void release();
    
private:
    Adafruit_PWMServoDriver pwm;
    int angleToPulse(float angle);
};

#endif
