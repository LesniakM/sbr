#ifndef BALANCE_MODULE_H
#define BALANCE_MODULE_H

#include <zephyr/types.h>

uint8_t readbuffer[12] = {};
float a_angle = 0;
float g_angle = 0;

float balance_value = 0;
float balance_value_P = 0;
float balance_value_D = 0;

float kP = 6.001;
float kI = 0.0;
float kD = 2.205;
float kD_dumping = 0.85F;
float gyro_favor_factor = 0.98F;

float complementary_angle = 0;
float current_angle = 0;

#endif // BALANCE_MODULE_H