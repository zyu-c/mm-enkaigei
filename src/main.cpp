#include "mouse.hpp"

void print_f(float a) {
    int data = ((float)a) * 1000;
    if (data < 0) {
        SCI_printf("-");
        data = -1 * data;
    }

    SCI_printf("%d.%03d", data / 1000, data % 1000);
}

int main() {
    Mouse* mouse = Mouse::getInstance();
    mouse->init();

    mouse->calibrateGyro();

    mouse->motor_vel_target[0] = 0.0;
    mouse->motor_vel_target[1] = 0.0;

    mouse->startControllerTimer();

    mouse->buzzer->on();
    while (1) {
        mouse->buzzer->setFreq(1000);
        mouse->ioex->set(0x00);
        delay_ms(50);
        mouse->buzzer->setFreq(3000);
        mouse->ioex->set(0x0F);
        delay_ms(50);
    }
    return 0;
}
