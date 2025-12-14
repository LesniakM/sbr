#include "usb_module.h"

LOG_MODULE_REGISTER(SBR_USB, LOG_LEVEL_DBG);

void usb_status_cb(enum usb_dc_status_code status, const uint8_t *param)
{
    switch (status)
    {
    case USB_DC_CONNECTED:
        LOG_INF("USB Connected");
        // Perform actions when USB is plugged in
        break;

    case USB_DC_DISCONNECTED:
        LOG_INF("USB Disconnected");
        // Perform actions when USB is unplugged
        break;

    default:
        break;
    }
}

int enable_usb(void)
{
    int ret = usb_enable(usb_status_cb);
    if (ret) {
        LOG_ERR("USB enable failed with code: %d", ret);
    } else {
        LOG_DBG("USB enabled.");
    }
    return ret;
}