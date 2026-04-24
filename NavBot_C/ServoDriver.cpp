#include "ServoDriver.h"

void ServoDriver::init() {
    Wire.begin(I2C_SDA_SERVO, I2C_SCL_SERVO);
    pwm.begin();
    pwm.setOscillatorFrequency(25000000);
    pwm.setPWMFreq(50); // 50Hz for Analog Servos
}

int ServoDriver::angleToPulse(float angle) {
    // 50Hz = 20ms period. 12-bit = 4096 steps.
    // 使用浮點運算保留精度: 0.5ms(102.4 units) 到 2.5ms(511.8 units)
    float pulse = 102.4 + (angle * (511.8 - 102.4) / 180.0);
    return (int)pulse;
}

void ServoDriver::setAngle(uint8_t channel, float angle) {
    pwm.setPWM(channel, 0, angleToPulse(angle));
}

void ServoDriver::release() {
    for (int i = 0; i < 16; i++) {
        pwm.setPWM(i, 0, 4096); // OFF
    }
}
