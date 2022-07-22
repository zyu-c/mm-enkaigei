#pragma once

#include <cstdint>

#include "buzzer.hpp"
#include "delay.hpp"
#include "encoder.hpp"
#include "gyro.hpp"
#include "ioex.hpp"
#include "motor.hpp"
#include "pid_speed_controller.hpp"
#include "sensor.hpp"

extern "C" {
#include "sci.h"
}

class Mouse {
   private:
    static Mouse* mouse_p;

    void initClock();
    void initPeripheral();
    void initControllerTimer();
    void checkBattery();

    void initVariable();

    PidSpeedController pid_motor[2];

   public:
    float yaw_ang_zero;
    float yaw_ang;
    float motor_vel[2];
    uint16_t enc_pos_prev[2];
    float machine_vel;

    float machine_vel_target;
    float machine_omega_target;
    float motor_vel_target[2];

    Buzzer* buzzer;
    Encoder* encoder;
    Gyro* gyro;
    Ioex* ioex;
    Motor* motor;
    Sensor* sensor;

    static Mouse* getInstance();
    void init();

    void control();

    void calibrateGyro();

    void startControllerTimer();
    void stopControllerTimer();
};
