#ifndef PERIPHALS_H
#define PERIPHALS_H

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

extern const struct gpio_dt_spec led;
extern const struct gpio_dt_spec mpu6500_address_pin;
extern const struct i2c_dt_spec dev_i2c;
extern const struct gpio_dt_spec m_driver_12_sleep;

extern bool drive;

void button1_pressed_cb(const struct device *dev, struct gpio_callback *cb,
                        uint32_t pins);

// void button2_pressed_cb(const struct device *dev, struct gpio_callback *cb,
// uint32_t pins);

/**
 * @brief Configures GPIO button callbacks for handling interrupts.
 *
 * This function initializes and configures the GPIO pins for button inputs,
 * sets up interrupts to trigger on both edges, and registers the corresponding
 * callback functions.
 *
 * Steps performed:
 * 1. Configures button1 as an input pin.
 * 2. Sets up an interrupt on both rising and falling edges for button1.
 * 3. Initializes a GPIO callback structure for button1.
 * 4. Registers the callback for button1 using `gpio_add_callback`.
 *
 * @note Code for button2 is commented out but can be enabled if needed.
 */
int config_buttons_callbacks();

/**
 * @brief Configures GPIO pins for the device
 *
 * Configures the LED pin as an active output.
 * Future implementation will configure MPU6500 address pin as active output
 * and motor driver sleep pin as inactive output.
 *
 * @return 0 on success, error code on failure.
 */
int configure_gpios();

void button1_cb(const struct device *dev, struct gpio_callback *cb,
                uint32_t pins);
void button1_pressed();
void button1_released();

#endif // PERIPHALS_H