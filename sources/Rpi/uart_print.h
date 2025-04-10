#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

void print(const char* ch);
void uart_init ( void );
void print_i(int i);
void print_d(double d);

#ifdef __cplusplus
}
#endif
