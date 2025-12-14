#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "periphals.h"
// #include "motors.h"
#include "mpu6050.h"

#include "balance_module.h"
#include "ble_comm.h"
#include "usb_module.h"

LOG_MODULE_REGISTER(SBR_main, LOG_LEVEL_DBG);

int time = 0;
bool drive = false;
uint8_t loop_time = 2;

void err_lock(int error_code) {
  LOG_ERR("Fatal error %d", error_code);
  while (1) {
    gpio_pin_toggle_dt(&led);
    k_msleep(200);
    gpio_pin_toggle_dt(&led);
    k_msleep(200);
  }
}

static inline void print_build_info() {
  LOG_INF("Build time: " __DATE__ " " __TIME__);
  LOG_INF("Build toolchain ver: " STRINGIFY(BUILD_VERSION));
  LOG_INF("START");
}

int main(void) {
  print_build_info();

  int err = 0;
  err += configure_gpios();
  err += config_buttons_callbacks();
  err += enable_usb();
  err += start_ble();
  err += init_mpu6050(&dev_i2c);

  if (err != 0) {
    LOG_ERR("Non zero error code detected in init: %d. Check logs.", err);
    err_lock(err);
  }

  // Poll if the DTR flag was set
  const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
  uint32_t dtr = 1; // 0;
  while (!dtr) {
    uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
    //    // Give CPU resources to low priority threads.
    k_msleep(100);
  }

  // pio_pin_set_dt(&m_driver_12_sleep, 1);
  uint8_t buffer[12];
  float current_angle = 0.0F;
  const int32_t dt_ms = 10;
  int index = 0;
  while (1) {
    index++;
    mpu6050_get_readings(&dev_i2c, buffer);

    int16_t accel_x = (buffer[0] << 8) | buffer[1];
    //int16_t accel_y = (buffer[2] << 8) | buffer[3];
    int16_t accel_z = (buffer[4] << 8) | buffer[5];
    //int16_t gyro_x = (buffer[8] << 8) | buffer[9];
    int16_t gyro_y = (buffer[10] << 8) | buffer[11];

    // Convert to physical units
    float accel_x_mss = convert_accel(accel_x);
    //float accel_y_mss = convert_accel(accel_y);
    float accel_z_mss = convert_accel(accel_z);
    float gyro_y_dps = convert_gyro(gyro_y);

    a_angle = accel_angle(accel_z_mss, accel_x_mss);;
    g_angle = gyro_angle(gyro_y_dps, dt_ms);

    current_angle = gyro_favor_factor * (current_angle + (g_angle * (1.00F / 100.0F))) + (1.00F - gyro_favor_factor) * a_angle;

    if (index >= 5) {
      index = 0;
      LOG_INF("Current angle: %.2f degrees", (double)current_angle);
      gpio_pin_set_dt(&led, 1);
      k_msleep(dt_ms);
    }
    else {
      gpio_pin_set_dt(&led, 0);
      k_msleep(dt_ms);
    }
  }
}
