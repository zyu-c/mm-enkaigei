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
}
