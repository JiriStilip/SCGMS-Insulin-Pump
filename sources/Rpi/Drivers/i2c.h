#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "peripherals.h"

#define I2C_FIFO_SIZE 16

#ifdef __cplusplus
extern "C" {
#endif

void i2c_init(void);
void i2c_write(uint8_t addr, uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif
#endif