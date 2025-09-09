#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include "periphals.h"
//#include "motors.h"
//#include "mpu6050.h"

#include "balance_module.h"

LOG_MODULE_REGISTER(SBR_main, LOG_LEVEL_DBG);

// USB
#include "usb_module.h"

// BLE
#include "ble_comm.h"

static const struct bt_le_adv_param *adv_param = BT_LE_ADV_PARAM(
	(BT_LE_ADV_OPT_CONNECTABLE |
	 BT_LE_ADV_OPT_USE_IDENTITY), /* Connectable advertising and use identity address */
	800, /* Min Advertising Interval 500ms (800*0.625ms) */
	801, /* Max Advertising Interval 500.625ms (801*0.625ms) */
	NULL); /* Set to NULL for undirected advertising */

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};

static const struct bt_data sd[] = {
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_SBR_SERVICE_VAL),
};

// BLE END


int time = 0;
bool drive = false;
uint8_t loop_time = 2;

int main(void)
{   
    LOG_INF("\nSTART");
    
    // TODO: Error handling.
    configure_gpios();
    LOG_DBG("GPIO configured.");
    config_buttons_callbacks();
    LOG_DBG("Button callback registered.");

    gpio_pin_toggle_dt(&led);
    k_msleep(200);
    gpio_pin_toggle_dt(&led);
    k_msleep(200);
    gpio_pin_toggle_dt(&led);
    k_msleep(200);
    
    k_msleep(10);

    int err = usb_enable(usb_status_cb);
    if (err)
    {
        LOG_ERR("USB failed with code: %d", err);
        return 0;
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
    if (err)
    {
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
    while (!dtr)
    {
        uart_line_ctrl_get(dev, UART_LINE_CTRL_DTR, &dtr);
    //    // Give CPU resources to low priority threads.
        k_msleep(100);
    }

    gpio_pin_toggle_dt(&led);

    //pio_pin_set_dt(&m_driver_12_sleep, 1);

    gpio_pin_toggle_dt(&led);
    k_msleep(200);
    gpio_pin_toggle_dt(&led);
    k_msleep(200);
    gpio_pin_toggle_dt(&led);
    k_msleep(200);

    while (1)
    {
        LOG_INF("hey");
        k_msleep(100);
        gpio_pin_toggle_dt(&led);
    }
}
