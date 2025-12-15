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

int64_t start = 0;
int64_t elapsed_ms =0;

#define SAMPLING_INTERVAL 5

K_SEM_DEFINE(i2c_tick_sem, 0, 1);

void timer_handler(struct k_timer *timer)
{
    k_sem_give(&i2c_tick_sem);   /* Signal only */
}

K_TIMER_DEFINE(i2c_timer, timer_handler, NULL);

void measurement_thread(void)
{
  while (1) {
    k_sem_take(&i2c_tick_sem, K_FOREVER);
    start = k_uptime_get();
    mpu6050_get_readings(&dev_i2c, readbuffer);

    int16_t accel_x = (readbuffer[0] << 8) | readbuffer[1];
    int16_t accel_z = (readbuffer[4] << 8) | readbuffer[5];
    int16_t gyro_y = (readbuffer[10] << 8) | readbuffer[11];

    float accel_x_mss = convert_accel(accel_x);
    float accel_z_mss = convert_accel(accel_z);
    float gyro_y_dps = convert_gyro(gyro_y);

    a_angle = accel_angle(accel_z_mss, accel_x_mss);
    g_angle = gyro_angle(gyro_y_dps, SAMPLING_INTERVAL);
    elapsed_ms = k_uptime_get() - start;
    current_angle = gyro_favor_factor * (current_angle + (g_angle * (1.00F / 100.0F))) + (1.00F - gyro_favor_factor) * a_angle;
  }
}

K_THREAD_DEFINE(i2c_tid, 1024, measurement_thread,
                NULL, NULL, NULL,
                5, 0, 0);
          
K_TIMER_DEFINE(measure_timer, timer_handler, NULL);

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

  k_timer_start(&measure_timer,
                  K_MSEC(SAMPLING_INTERVAL),   /* initial delay */
                  K_MSEC(SAMPLING_INTERVAL));  /* period */
  
  while (1) {
    LOG_INF("Current angle: %.2f A: %.2f G: %.2f Time: %d", (double)current_angle, (double)a_angle, (double)g_angle, (int)elapsed_ms);
    gpio_pin_set_dt(&led, 1);
    k_msleep(1);
    //LOG_INF("Loop took %lld ms\n", elapsed_ms);
    gpio_pin_set_dt(&led, 0);
    k_msleep(19);
  }
}
