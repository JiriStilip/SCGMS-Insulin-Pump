#ifndef GPIO_IRQ_H
#define GPIO_IRQ_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

#define INSET_PLUS  (1 << 0)
#define INSET_MINUS (1 << 1)
#define INSET_SET   (1 << 2)
#define AUXIN_BTN1  (1 << 3)
#define AUXIN_BTN2  (1 << 4)

extern TaskHandle_t inset_task_handle;
extern TaskHandle_t auxin_task_handle;
extern TaskHandle_t cgm_task_handle;

#ifdef __cplusplus
extern "C" {
#endif

void gpio_irq_init(void);
void gpio_irq_isr(unsigned int irq, void *pParam);

#ifdef __cplusplus
}
#endif
#endif