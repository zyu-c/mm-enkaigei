#include "pid_position_controller.hpp"

void PidPositionController::setParam(float kp, float ki, float kd, float max,
                                     float min, float freq) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->max = max;
    this->min = min;
    this->freq = freq;
    reset();
}

void PidPositionController::reset() {
    e = 0.0;
    e_int = 0.0;
    e_diff = 0.0;
    u = 0.0;
}

void PidPositionController::update(float target, float current) {
    e_diff = freq * ((target - current) - e);
    e = target - current;
    e_int = e_int + e / freq;

    u = kp * e + ki * e_int + kd * e_diff;

    if (u < min) {
        u = min;
    } else if (max < u) {
        u = max;
    }
}

float PidPositionController::getOutput() { return u; }
