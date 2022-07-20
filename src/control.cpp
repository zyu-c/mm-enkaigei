#include "mouse.hpp"

void Mouse::control() {
    // これを入れると多重割り込みが動く
    // 実質おまじない
    __builtin_rx_setpsw('I');

    encoder->update();
    gyro->update();
    ioex->update();

    yaw_ang = (float)yaw_ang +
              ((float)(gyro->getZAngVel()) - (float)yaw_ang_zero) / 1000.0;

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
}
