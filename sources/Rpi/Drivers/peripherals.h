#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <stdint.h>

#define PERIPHERAL_BASE 0x20000000

#define TIMER_BASE  (PERIPHERAL_BASE + 0x3000)

#define REG_TIMER_CLO   *((volatile uint32_t *)(TIMER_BASE + 0x04))

#define GPIO_BASE   (PERIPHERAL_BASE + 0x200000)

#define REG_GPIO_GPSET_ADDR  (GPIO_BASE + 0x1C)
#define REG_GPIO_GPCLR_ADDR  (GPIO_BASE + 0x28)
#define REG_GPIO_GPLEV_ADDR  (GPIO_BASE + 0x34)
#define REG_GPIO_GPEDS_ADDR  (GPIO_BASE + 0x40)
#define REG_GPIO_GPREN_ADDR  (GPIO_BASE + 0x4C)

#define GPIO_FUNCTION_INPUT     0b000
#define GPIO_FUNCTION_OUTPUT    0b001
#define GPIO_FUNCTION_ALT5      0b010
#define GPIO_FUNCTION_ALT4      0b011
#define GPIO_FUNCTION_ALT0      0b100
#define GPIO_FUNCTION_ALT1      0b101
#define GPIO_FUNCTION_ALT2      0b110
#define GPIO_FUNCTION_ALT3      0b111

#define BSC1_BASE   (PERIPHERAL_BASE + 0x804000)

#define REG_BSC1_C    *((volatile uint32_t *)(BSC1_BASE + 0x00))
#define REG_BSC1_S    *((volatile uint32_t *)(BSC1_BASE + 0x04))
#define REG_BSC1_DLEN *((volatile uint32_t *)(BSC1_BASE + 0x08))
#define REG_BSC1_A    *((volatile uint32_t *)(BSC1_BASE + 0x0C))
#define REG_BSC1_FIFO *((volatile uint32_t *)(BSC1_BASE + 0x10))

#define BSC_C_I2CEN    (1 << 15)    // I2C Enable
#define BSC_C_ST       (1 << 7)     // Start Transfer
#define BSC_C_CLEAR    (1 << 4)     // Clear
#define BSC_C_READ     (1 << 0)     // Read Transfer

#define BSC_S_CLKT     (1 << 9)     // Clock Stretch Timeout
#define BSC_S_ERR      (1 << 8)     // ACK Error
#define BSC_S_DONE     (1 << 1)     // Transfer Done
#define BSC_S_TA       (1 << 0)     // Transfer Active

#endif