#ifndef BLE_COMM
#define BLE_COMM

#include <zephyr/types.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

// GATT Service UUID
#define BT_UUID_SBR_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x00000001, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief Button Characteristic UUID. */
#define BT_UUID_SBR_ACCEL_VAL \
    BT_UUID_128_ENCODE(0x00000002, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief Button Characteristic UUID. */
#define BT_UUID_SBR_GYRO_VAL \
    BT_UUID_128_ENCODE(0x00000003, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

/** @brief Control Characteristic UUID. */
#define BT_UUID_SBR_CONTROL_VAL \
    BT_UUID_128_ENCODE(0x00000011, 0x1212, 0xefde, 0x1523, 0x785feabcd123)

#define BT_UUID_SBR_SERVICE BT_UUID_DECLARE_128(BT_UUID_SBR_SERVICE_VAL)
#define BT_UUID_SBR_ACCEL BT_UUID_DECLARE_128(BT_UUID_SBR_ACCEL_VAL)
#define BT_UUID_SBR_GYRO BT_UUID_DECLARE_128(BT_UUID_SBR_GYRO_VAL)
#define BT_UUID_SBR_CONTROL BT_UUID_DECLARE_128(BT_UUID_SBR_CONTROL_VAL)

/** @brief Callback type for when an LED state change is received. */
typedef void (*led_cb_t)(const bool led_state);

/** @brief Callback type for when the button state is pulled. */
typedef bool (*button_cb_t)(void);

struct sbr_callback
{
    /** LED state change callback. */
    led_cb_t led_cb;
    /** Button read callback. */
    button_cb_t button_cb;
};

ssize_t read_accel(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
ssize_t read_gyro(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);
ssize_t update_control(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, uint16_t len, uint16_t offset, uint8_t flags);
int ble_init(struct sbr_callback *callbacks);

void on_connected(struct bt_conn *conn, uint8_t err);

void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);

void on_disconnected(struct bt_conn *conn, uint8_t reason);

extern struct bt_conn_cb connection_callbacks;

void sbr_contrl_cb(bool state);
bool sbr_accel_cb(void);
bool sbr_gyro_cb(void);

extern struct sbr_callback sbr_callbacks;

#endif