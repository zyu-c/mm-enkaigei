#include "pid_speed_controller.hpp"

void PidSpeedController::setParam(float kp, float ki, float kd, float max,
                                  float min, float freq) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->max = max;
    this->min = min;
    this->freq = freq;
    reset();
}

void PidSpeedController::reset() {
    e = 0.0;
    e_int = 0.0;
    e_diff = 0.0;
    m = 0.0;
    dm = 0.0;
    u = 0.0;
}

void PidSpeedController::update(float target, float current) {
    e_diff = freq * ((target - current) - e);
    e = target - current;
    e_int = e_int + e / freq;

    dm = freq * (kp * e + ki * e_int + kd * e_diff - m);
    m = kp * e + ki * e_int + kd * e_diff;

    u = u + dm;

    if (u < min) {
        u = min;
    } else if (max < u) {
        u = max;
    }
}

float PidSpeedController::getOutput() { return u; }
