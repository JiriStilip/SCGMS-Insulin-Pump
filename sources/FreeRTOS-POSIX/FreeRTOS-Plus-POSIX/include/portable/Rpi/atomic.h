#define ATOMIC_COMPARE_AND_SWAP_SUCCESS 1
#include <stdint.h>
uint32_t 	Atomic_CompareAndSwap_u32 (uint32_t volatile *pDestination, uint32_t ulExchange, uint32_t ulComparand);
uint32_t 	Atomic_Increment_u32 (uint32_t volatile *pAddend);
uint32_t 	Atomic_Decrement_u32 (uint32_t volatile *pAddend);
