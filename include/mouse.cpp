#include "mouse.hpp"

extern "C" {
#include <cstdlib>

#include "iodefine.h"
}

Mouse* Mouse::mouse_p = nullptr;

Mouse* Mouse::getInstance() {
    if (!mouse_p) {
        mouse_p = (Mouse*)malloc(sizeof(Mouse));
    }

    return mouse_p;
}

void Mouse::init() {
    initClock();
    init_sci();
    initPeripheral();
    initDelayTimer();
    initControllerTimer();
    initVariable();

    checkBattery();

    buzzer->on();
    for (int i = 0; i < 30; i++) {
        if (i % 5 == 0) {
            buzzer->setFreq(3000);
        } else {
            buzzer->setFreq(2000);
        }

        ioex->set(i);
        ioex->update();
        delay_ms(50);
    }
    buzzer->off();
    ioex->set(0x00);
    ioex->update();

    machine_vel_target = 0.0;
    machine_omega_target = 0.0;

    pid_motor[0].setParam(0.000002, 0.0001, 0.0, 0.8, -0.8, 1000.0);
    pid_motor[1].setParam(0.000002, 0.0001, 0.0, 0.8, -0.8, 1000.0);
    pid_ang.setParam(5.0, 0.0, 0.0, 5.0, -5.0, 1000.0);
    machine_theta_target = 0.0;
}

void Mouse::initClock() {
    //プロテクト解除
    SYSTEM.PRCR.WORD = 0xA50b;

    // 分周器設定
    // 12MHz -> 192MHz
    SYSTEM.PLLCR.WORD = 0x0F00;

    // PLL有効化
    SYSTEM.PLLCR2.BYTE = 0x00;

    // PLL安定まで待機
    SYSTEM.PLLWTCR.BYTE = 0x0F;

    // 周辺回路動作周波数設定
    // FCLK : 1/4（FlashIF，最大50MHz）
    // ICLK : 1/2（システムクロック，最大100MHz）
    // BCLK : 停止（外部バスクロック）
    // SDCLK : 停止（SDRAMクロック）
    // PCLKA : 1/2（周辺モジュールクロックA，最大100MHz）
    // PCLKB = PCLK : 1/4（周辺モジュールクロックB，最大50MHz）
    // UCLK : 1/4（USBクロック）
    // IECLK : 1/4（IEBUSクロック）
    // クロックソース：PLL
    SYSTEM.SCKCR.LONG = 0x21C21211;
    SYSTEM.SCKCR2.WORD = 0x0032;
    SYSTEM.BCKCR.BYTE = 0x01;
    SYSTEM.SCKCR3.WORD = 0x0400;

    // 再プロテクト
    SYSTEM.PRCR.WORD = 0xA500;
}

void Mouse::initPeripheral() {
    buzzer = Buzzer::getInstance();
    buzzer->init();
    buzzer->setFreq(3000);

    encoder = Encoder::getInstance();
    encoder->init();

    gyro = Gyro::getInstance();
    gyro->init();

    ioex = Ioex::getInstance();
    ioex->init();
    ioex->set(0x00);

    motor = Motor::getInstance();
    motor->init();
    motor->setDuty(0, 0.0);
    motor->setDuty(1, 0.0);

    sensor = Sensor::getInstance();
    sensor->init();
    sensor->ledOff();
}

void Mouse::initControllerTimer() {
    // CMT1有効化
    SYSTEM.PRCR.WORD = 0xA502;
    MSTP(CMT1) = 0;
    SYSTEM.PRCR.WORD = 0xA500;

    // 割り込みを許可
    CMT1.CMCR.BIT.CMIE = 1;
    ICU.IR[29].BIT.IR = 0;
    ICU.IPR[5].BIT.IPR = 14;
    ICU.IER[3].BIT.IEN5 = 1;

    // 128分周
    // 375Clkで1ms
    CMT1.CMCR.BIT.CKS = 2;

    // 1msごとに割り込み
    // 制御周期 1kHz
    CMT1.CMCOR = 375 - 1;
}

void Mouse::checkBattery() {
    sensor->updateBatteryVoltage();

    SCI_printf("Battry Voltage = %d.%02d V\n",
               sensor->getBatteryVoltage() / 100,
               sensor->getBatteryVoltage() % 100);

    if (sensor->isLowBattery()) {
        buzzer->setFreq(1000);
        while (1) {
            buzzer->on();
            delay_ms(100);
            buzzer->off();
            delay_ms(100);
        }
    }
}

void Mouse::initVariable() {
    yaw_ang = 0.0;

    encoder->update();

    for (int i = 0; i < 2; i++) {
        enc_pos_prev[i] = encoder->getPosition(i);
        motor_vel[i] = 0.0;
    }
}

void Mouse::calibrateGyro() {
    float tmp = 0.0;
    for (int i = 0; i < 1000; i++) {
        gyro->update();
        tmp = (float)tmp + (float)(gyro->getZAngVel());
    }
    yaw_ang_zero = ((float)tmp / 1000.0);
}

void Mouse::startControllerTimer() {
    initVariable();
    CMT1.CMCNT = 0;
    CMT.CMSTR0.BIT.STR1 = 1;
}

void Mouse::stopControllerTimer() { CMT.CMSTR0.BIT.STR1 = 0; }
