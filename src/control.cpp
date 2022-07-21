#include "mouse.hpp"

void Mouse::control() {
    // これを入れると多重割り込みが動く
    // 実質おまじない
    __builtin_rx_setpsw('I');

    encoder->update();
    gyro->update();
    ioex->update();

    // Yaw角更新
    yaw_ang = (float)yaw_ang +
              ((float)(gyro->getZAngVel()) - (float)yaw_ang_zero) / 1000.0;

    // モータ速度更新
    int16_t enc_diff;
    for (int i = 0; i < 2; i++) {
        enc_diff = encoder->getPosition(i) - enc_pos_prev[i];
        enc_pos_prev[i] = encoder->getPosition(i);

        if (enc_diff < -512) {
            motor_vel[i] = enc_diff + 1024;
        } else if (enc_diff > 512) {
            motor_vel[i] = enc_diff - 1024;
        } else {
            motor_vel[i] = enc_diff;
        }
        motor_vel[i] = motor_vel[i] / 1024.0 * 12.2 * 3.14 * 1000.0;
    }

    machine_vel = (motor_vel[0] + motor_vel[1]) / 2.0;

    // 速度型PID
    for (int i = 0; i < 2; i++) {
        error_diff[i] =
            1000.0 * (motor_vel_target[i] - motor_vel[i] - error[i]);
        error_int[i] =
            0.001 * (motor_vel_target[i] - motor_vel[i]) + error_int[i];
        error[i] = motor_vel_target[i] - motor_vel[i];

        output_duty[i] =
            output_duty[i] + 1000.0 * (kp * error[i] + ki * error_int[i] +
                                       kd * error_diff[i] - m[i]);
        m[i] = kp * error[i] + ki * error_int[i] + kd * error_diff[i];

        // P制御のみ
        // error[i] = motor_vel_target[i] - motor_vel[i];
        // output_duty[i] = output_duty[i] + 1000.0 * (kp * error[i] - m[i]);
        // m[i] = (float)((float)kp * (float)error[i]);

        if (output_duty[i] < -0.8) {
            output_duty[i] = -0.8;
        } else if (output_duty[i] > 0.8) {
            output_duty[i] = 0.8;
        }
    }

    motor->setDuty(0, output_duty[0]);
    motor->setDuty(1, output_duty[1]);
}
