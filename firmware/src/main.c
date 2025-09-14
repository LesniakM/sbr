#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "periphals.h"
// #include "motors.h"
// #include "mpu6050.h"

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

int main(void) {
  LOG_INF("Build time: " __DATE__ " " __TIME__);
  LOG_INF("Build toolchain ver: " STRINGIFY(BUILD_VERSION));
  LOG_INF("START");
  int err;

  err = configure_gpios();
  if (err) {
    LOG_ERR("GPIO config failes with error: %d", err);
    err_lock(err);
  }
  LOG_DBG("GPIO configured.");

  err = config_buttons_callbacks();
  if (err) {
    LOG_ERR("Button callback config failed with error: %d", err);
    err_lock(err);
  }
  LOG_DBG("Button callback registered.");

  err = usb_enable(usb_status_cb);
  if (err) {
    LOG_ERR("USB failed with code: %d", err);
    err_lock(err);
  }
  LOG_DBG("USB enabled.");

  k_msleep(50);

  LOG_DBG("Initializing Bluetooth...");
  err = bt_enable(NULL);
  if (err) {
    LOG_ERR("Bluetooth init failed (err %d)", err);
    return -1;
  }
  LOG_DBG("Bluetooth initialized.");

  bt_conn_cb_register(&connection_callbacks);
  LOG_DBG("cb registered.");

  err = ble_init(&sbr_callbacks);
  if (err) {
    LOG_ERR("Failed to init LBS (err:%d)", err);
    return -1;
  }
  LOG_DBG("ble_init ok.");

  err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
  if (err) {
    LOG_ERR("Advertising failed to start (err %d)", err);
    return -1;
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

  while (1) {
    LOG_INF("hey");
    gpio_pin_set_dt(&led, 1);
    k_msleep(50);
    gpio_pin_set_dt(&led, 0);
    k_msleep(950);
  }
}
