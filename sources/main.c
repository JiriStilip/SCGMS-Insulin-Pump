#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <scgms.h>
#include <uart_print.h>
#include <i2c.h>
#include <gpio.h>
#include <gpio_irq.h>

void _init(void) {;}

void main (void)
{
	#pragma GCC diagnostic push 

	#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
 	__libc_init_array();
	#pragma GCC diagnostic pop

	gpio_set_function(PIN_LED_UPLOAD, GPIO_FUNCTION_OUTPUT);
	gpio_set_output(PIN_LED_UPLOAD, HIGH);

    uart_init();
	i2c_init();
	build_filter_chain(NULL);

	gpio_irq_init();

	vTaskStartScheduler();
}





