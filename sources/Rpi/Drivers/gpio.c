#include "gpio.h"

void gpio_set_function(uint8_t pin, uint8_t func)
{
    uint32_t gpfsel_addr = GPIO_BASE + ((pin / 10) * 0x04);
    uint32_t gpfsel_bit = (pin % 10) * 3;

    uint32_t gpfsel_reg = *((volatile uint32_t *) gpfsel_addr);
    gpfsel_reg &= ~(7 << gpfsel_bit);
    gpfsel_reg |= (func << gpfsel_bit);
    *((volatile uint32_t *) gpfsel_addr) = gpfsel_reg;
}