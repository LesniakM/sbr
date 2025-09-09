#ifndef BALANCE_MODULE_H
#define BALANCE_MODULE_H

#include <zephyr/types.h>

uint8_t readbuffer[12] = {};
int16_t correction_values[6] = {0, 0, 0, 0, 0, 0};
int16_t raw_values[6] = {0, 0, 0, 0, 0, 0};
int64_t cumulated_values[6] = {0, 0, 0, 0, 0, 0};
float imu_values[5] = {0, 0, 0, 0, 0};
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

#endif // BALANCE_MODULE_H