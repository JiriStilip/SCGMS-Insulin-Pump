#include "i2c.h"

#include "gpio.h"
#include "peripherals.h"

void i2c_init(void)
{
    // Set GPIO2 and GPIO3 functions to ALT0
    gpio_set_function(PIN_I2C1_SDA, GPIO_FUNCTION_ALT0);
    gpio_set_function(PIN_I2C1_SCL, GPIO_FUNCTION_ALT0);

    // Set I2C enable
    REG_BSC1_C |= BSC_C_I2CEN;
}

void i2c_write(uint8_t addr, uint8_t *data, uint8_t len)
{
    // Set destination address and data length
    REG_BSC1_A = addr;
    REG_BSC1_DLEN = len;
    // "Push" data into BSC FIFO (maximum 16 bytes in BCM2835, rest is ignored)
    for (uint8_t i = 0; i < len; ++i)
    {
        REG_BSC1_FIFO = data[i];
    }
    // Clear Status register flags
    REG_BSC1_S |= BSC_S_CLKT | BSC_S_ERR | BSC_S_DONE;
    // Set start bit in Control register
    REG_BSC1_C |= BSC_C_I2CEN | BSC_C_ST;
    // Wait for the transfer to finish
    volatile uint32_t s = REG_BSC1_S;
    while ( !(s & BSC_S_DONE) ) 
    {
        s = REG_BSC1_S;
    }
}