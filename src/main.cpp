#include "mouse.hpp"

void print_f(float a) {
    int data = ((float)a) * 1000;
    if (data < 0) {
        SCI_printf("-");
        data = -1 * data;
    }

    SCI_printf("%d.%03d\n", data / 1000, data % 1000);
}

int main() {
    Mouse* mouse = Mouse::getInstance();
    mouse->init();
    mouse->calibrateGyro();

    mouse->startControllerTimer();

    while (1) {
        print_f(mouse->yaw_ang * 180.0 / 3.14);
        delay_ms(10);
    }
    return 0;
}
