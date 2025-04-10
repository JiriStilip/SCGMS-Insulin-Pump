#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <scgms.h>
#include <uart_print.h>

void _init(void) {;}

void main (void)
{
	#pragma GCC diagnostic push 

	#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
 	__libc_init_array();
	#pragma GCC diagnostic pop

    uart_init();
	build_filter_chain(NULL);
	vTaskStartScheduler();
}





