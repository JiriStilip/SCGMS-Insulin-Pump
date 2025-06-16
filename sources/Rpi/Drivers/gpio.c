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

void gpio_set_output(uint8_t pin, uint8_t level)
{
    uint32_t reg_addr = ((level == 0) ? REG_GPIO_GPCLR_ADDR : REG_GPIO_GPSET_ADDR);
    reg_addr += ((pin < 32) ? 0 : 4);
    uint32_t reg_bit = (pin % 32);

    *((volatile uint32_t *) reg_addr) = (1 << reg_bit);
}

uint8_t gpio_get_input(uint8_t pin)
{
    uint32_t gplev_addr = REG_GPIO_GPLEV_ADDR + ((pin / 32) * 0x04);
    uint32_t gplev_bit = (pin % 32);

    uint32_t gplev_reg = *((volatile uint32_t *) gplev_addr);

    return ((gplev_reg >> gplev_bit) & 0x1) ? 1 : 0;
}

void gpio_detect_rising(uint8_t pin)
{
    uint32_t gpren_addr = REG_GPIO_GPREN_ADDR + ((pin / 32) * 0x04);
    uint32_t gpren_bit = (pin % 32);

    *((volatile uint32_t *) gpren_addr) |= (1 << gpren_bit);
}

uint8_t gpio_detect_status(uint8_t pin)
{
    uint32_t gpeds_addr = REG_GPIO_GPEDS_ADDR + ((pin / 32) * 0x04);
    uint32_t gpeds_bit = (pin % 32);

    uint32_t gpeds_reg = *((volatile uint32_t *) gpeds_addr);

    return ((gpeds_reg >> gpeds_bit) & 0x1) ? 1 : 0;
}

void gpio_detect_status_clear(uint8_t pin)
{
    uint32_t gpeds_addr = REG_GPIO_GPEDS_ADDR + ((pin / 32) * 0x04);
    uint32_t gpeds_bit = (pin % 32);

    *((volatile uint32_t *) gpeds_addr) = (1 << gpeds_bit);
}