#include "gpio_irq.h"

#include "stdlib.h"
#include "gpio.h"
#include "irq.h"
#include "bcm2835_intc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"

#define DEBOUNCE_US 200000

TaskHandle_t inset_task_handle = NULL;
TaskHandle_t auxin_task_handle = NULL;
TaskHandle_t cgm_task_handle = NULL;

void gpio_irq_init(void)
{
    gpio_set_function(PIN_ROTARY_S1, GPIO_FUNCTION_INPUT);
    gpio_set_function(PIN_ROTARY_S2, GPIO_FUNCTION_INPUT);
    gpio_set_function(PIN_ROTARY_KEY, GPIO_FUNCTION_INPUT);
    gpio_set_function(PIN_BUTTON_CGM_FAULT, GPIO_FUNCTION_INPUT);
    gpio_set_function(PIN_BUTTON_INSULIN_REFILL, GPIO_FUNCTION_INPUT);

    gpio_detect_rising(PIN_ROTARY_S1);
    gpio_detect_status_clear(PIN_ROTARY_S1);

    gpio_detect_rising(PIN_ROTARY_KEY);
    gpio_detect_status_clear(PIN_ROTARY_KEY);

    gpio_detect_rising(PIN_BUTTON_CGM_FAULT);
    gpio_detect_status_clear(PIN_BUTTON_CGM_FAULT);

    gpio_detect_rising(PIN_BUTTON_INSULIN_REFILL);
    gpio_detect_status_clear(PIN_BUTTON_INSULIN_REFILL);

    irqRegister(BCM2835_IRQ_ID_GPIO_0, gpio_irq_isr, NULL);
    irqEnable(BCM2835_IRQ_ID_GPIO_0);
}

void gpio_irq_isr(unsigned int irq, void *pParam)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    static uint32_t last_time = 0;
    uint32_t now = micros();

    uint32_t notify_value = 0;
    uint8_t s2_state;

    if (gpio_detect_status(PIN_ROTARY_S1)) 
    {
        gpio_detect_status_clear(PIN_ROTARY_S1);

        // direction
        s2_state = gpio_get_input(PIN_ROTARY_S2);
        
        // debounce check
        if ((now - last_time) < DEBOUNCE_US) {
            return;
        }
        last_time = now;

        if (s2_state == 0)
        {
            // notification with direction clockwise
            notify_value |= INSET_PLUS;
        }
        else 
        {
            // notification with direction counter-clockwise
            notify_value |= INSET_MINUS;
        }
        xTaskNotifyFromISR(inset_task_handle, notify_value, eSetBits, &xHigherPriorityTaskWoken);
    }

    if (gpio_detect_status(PIN_ROTARY_KEY))
    {
        gpio_detect_status_clear(PIN_ROTARY_KEY);

        // debounce check
        if ((now - last_time) < DEBOUNCE_US) {
            return;
        }
        last_time = now;

        notify_value |= INSET_SET;

        xTaskNotifyFromISR(inset_task_handle, notify_value, eSetBits, &xHigherPriorityTaskWoken);
    }

    if (gpio_detect_status(PIN_BUTTON_CGM_FAULT))
    {
        gpio_detect_status_clear(PIN_BUTTON_CGM_FAULT);

        // debounce check
        if ((now - last_time) < DEBOUNCE_US) {
            return;
        }
        last_time = now;

        notify_value |= AUXIN_BTN1;

        xTaskNotifyFromISR(auxin_task_handle, notify_value, eSetBits, &xHigherPriorityTaskWoken);
    }

    if (gpio_detect_status(PIN_BUTTON_INSULIN_REFILL))
    {
        gpio_detect_status_clear(PIN_BUTTON_INSULIN_REFILL);

        // debounce check
        if ((now - last_time) < DEBOUNCE_US) {
            return;
        }
        last_time = now;

        notify_value |= AUXIN_BTN2;

        xTaskNotifyFromISR(auxin_task_handle, notify_value, eSetBits, &xHigherPriorityTaskWoken);
    }
}