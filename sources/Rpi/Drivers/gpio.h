#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "peripherals.h"

#define LOW     0
#define HIGH    1

#define PIN_I2C1_SDA                2
#define PIN_I2C1_SCL                3
#define PIN_DHT11_DATA              4
#define PIN_UART_TX                 14
#define PIN_UART_RX                 15
#define PIN_LED_WATCHDOG            17
#define PIN_LED_UPLOAD              18
#define PIN_BUTTON_CGM_FAULT        22
#define PIN_ROTARY_S1               23
#define PIN_ROTARY_S2               24
#define PIN_ROTARY_KEY              25
#define PIN_BUTTON_INSULIN_REFILL   27

#ifdef __cplusplus
extern "C" {
#endif

void gpio_set_function(uint8_t pin, uint8_t func);

void gpio_set_output(uint8_t pin, uint8_t level);
uint8_t gpio_get_input(uint8_t pin);

void gpio_detect_rising(uint8_t pin);
uint8_t gpio_detect_status(uint8_t pin);
void gpio_detect_status_clear(uint8_t pin);

#ifdef __cplusplus
}
#endif
#endif