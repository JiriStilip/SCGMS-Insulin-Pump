#include "atomic.h"
uint32_t 	Atomic_CompareAndSwap_u32 (uint32_t volatile *pDestination, uint32_t ulExchange, uint32_t ulComparand)
{return 1;}
uint32_t 	Atomic_Increment_u32 (uint32_t volatile *pAddend)
{return 1;}
uint32_t 	Atomic_Decrement_u32 (uint32_t volatile *pAddend)
{return 1;}
