#include "ble_comm.h"

LOG_MODULE_REGISTER(SBR_BLE, LOG_LEVEL_DBG);

static struct sbr_callback sbr_cb;

static bool button_state;

BT_GATT_SERVICE_DEFINE(sbr_service,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_SBR_SERVICE),

                       BT_GATT_CHARACTERISTIC(BT_UUID_SBR_ACCEL,
                                              BT_GATT_CHRC_READ,
                                              BT_GATT_PERM_READ, read_accel, NULL,
                                              &button_state),

                       BT_GATT_CHARACTERISTIC(BT_UUID_SBR_GYRO,
                                              BT_GATT_CHRC_READ,
                                              BT_GATT_PERM_READ, read_gyro, NULL,
                                              &button_state),

                       BT_GATT_CHARACTERISTIC(BT_UUID_SBR_CONTROL,
                                              BT_GATT_CHRC_WRITE,
                                              BT_GATT_PERM_WRITE,
                                              NULL, update_control, NULL));

ssize_t read_accel(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                   uint16_t len, uint16_t offset)
{
    // get a pointer to button_state which is passed in the BT_GATT_CHARACTERISTIC() and stored in attr->user_data
    const char *value = attr->user_data;

    // LOG_DBG("Attribute read, handle: %u, conn: %p", attr->handle, (void *)conn);

    if (sbr_cb.button_cb)
    {
        // Call the application callback function to update the get the current value of the button
        button_state = sbr_cb.button_cb();
        return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
    }

    return 0;
}

ssize_t read_gyro(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf,
                  uint16_t len, uint16_t offset)
{
    // get a pointer to button_state which is passed in the BT_GATT_CHARACTERISTIC() and stored in attr->user_data
    const char *value = attr->user_data;

    // LOG_DBG("Attribute read, handle: %u, conn: %p", attr->handle, (void *)conn);

    if (sbr_cb.button_cb)
    {
        // Call the application callback function to update the get the current value of the button
        button_state = sbr_cb.button_cb();
        return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
    }

    return 0;
}

ssize_t update_control(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf,
                       uint16_t len, uint16_t offset, uint8_t flags)
{
    // LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

    if (len != 1U)
    {
        // LOG_DBG("Write led: Incorrect data length");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (offset != 0)
    {
        // LOG_DBG("Write led: Incorrect data offset");
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (sbr_cb.led_cb)
    {
        // Read the received value
        uint8_t val = *((uint8_t *)buf);

        if (val == 0x00 || val == 0x01)
        {
            // Call the application callback function to update the LED state
            sbr_cb.led_cb(val ? true : false);
        }
        else
        {
            // LOG_DBG("Write led: Incorrect value");
            return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
        }
    }

    return len;
}

int ble_init(struct sbr_callback *callbacks)
{
	if (callbacks) {
		sbr_cb.led_cb = callbacks->led_cb;
		sbr_cb.button_cb = callbacks->button_cb;
	}

	return 0;
}


void on_connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		LOG_ERR("Connection failed (err %u)", err);
		return;
	}

	LOG_INF("Connected\n");
}

void on_le_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    double connection_interval = interval*1.25;         // in ms
    uint16_t supervision_timeout = timeout*10;          // in ms
    LOG_INF("Connection parameters updated: interval %.2f ms, latency %d intervals, timeout %d ms", connection_interval, latency, supervision_timeout);
}

void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason %u)", reason);
}

struct bt_conn_cb connection_callbacks = {
	.connected = on_connected,
    .le_param_updated = on_le_param_updated,
	.disconnected = on_disconnected,
};


void sbr_contrl_cb(bool state)
{
    // dk_set_led(USER_LED, led_state);
}

bool sbr_accel_cb(void)
{
    return false;
}

bool sbr_gyro_cb(void)
{
    return true;
}

struct sbr_callback sbr_callbacks = {
    .led_cb = sbr_contrl_cb,
    .button_cb = sbr_accel_cb,
};