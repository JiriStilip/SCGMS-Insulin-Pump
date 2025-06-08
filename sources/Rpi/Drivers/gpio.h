#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "peripherals.h"

#ifdef __cplusplus
extern "C" {
#endif

void gpio_set_function(uint8_t pin, uint8_t func);

#ifdef __cplusplus
}
#endif
#endif