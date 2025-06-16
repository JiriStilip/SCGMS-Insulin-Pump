#include "timer.h"

uint32_t micros(void)
{
    return REG_TIMER_CLO;
}