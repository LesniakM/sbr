#include "mpu6050.h"
#include <math.h>

LOG_MODULE_REGISTER(SBR_MPU6050, LOG_LEVEL_DBG);

int init_mpu6050(const struct i2c_dt_spec *dev_i2c)
{   
    int err = 0;
    err += i2c_reg_write_byte_dt(dev_i2c, MPU6050_REG_PWR_MGMT_1, 0);
    LOG_DBG("%d\n", err);
    k_msleep(2);
    err += i2c_reg_write_byte_dt(dev_i2c, MPU6050_REG_ACCEL_CONFIG, 0x0);
    LOG_DBG("%d\n", err);
    k_msleep(2);
    err += i2c_reg_write_byte_dt(dev_i2c, MPU6050_REG_GYRO_CONFIG, 0x0);
    LOG_DBG("%d\n", err);
    k_msleep(2);
    err += i2c_reg_write_byte_dt(dev_i2c, MPU6050_REG_CONFIG, BW_42HZ);
    LOG_DBG("%d\n", err);
    k_msleep(2);
    LOG_DBG("MPU6050 init end with code: %d\n", err);
    return err;
}

/*
From MPU6050 datasheet:
AFS_SEL Full Scale Range LSB Sensitivity
0	±2g 	16384 	LSB/g
1	±4g		8192 	LSB/g
2	±8g 	4096 	LSB/g
3	±16g 	2048 	LSB/g
Returns in m/ss
*/
float convert_accel(int16_t raw_val)
{
    return (float)raw_val / 16384.F * G_CONSTANT;
}

float accel_angle(float x_acc, float z_acc)
{
    return atan2f(x_acc, -z_acc) * (180.0F / M_PI);
}

float gyro_angle(float angle_speed, float time_ms)
{
    return -angle_speed * time_ms / 1000.F;
}

/*
From MPU6050 datasheet:
 * FS_SEL Full Scale Range LSB Sensitivity
 * 0 ± 250 °/s 131 LSB/°/s     131072  (2^17)
 * 1 ± 500 °/s 65.5 LSB/°/s    65 536  (2^16)
 * 2 ± 1000 °/s 32.8 LSB/°/s   32 768  (2^15)
 * 3 ± 2000 °/s 16.4 LSB/°/s   16 384  (2^14)
 * Return result in degree/s
*/
float convert_gyro(int16_t raw_val)
{
    return (float)raw_val / 131.072F;
}

void mpu6050_get_readings(const struct i2c_dt_spec *dev_i2c, uint8_t *buffer)
{
    i2c_burst_read_dt(dev_i2c, (uint8_t)MPU6050_REG_ACCEL_XOUT_H, buffer, 12);
}