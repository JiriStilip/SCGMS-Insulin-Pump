#include "uart_print.h"
#include "ftoa.h"
#include "stdlib.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "semphr.h"

#define PBASE 0x20000000

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044)
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)

#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)

SemaphoreHandle_t uartMutex;


void write32(unsigned int address, unsigned int val)
{
    *((volatile unsigned int*)address) = val;
}

unsigned int read32(unsigned int address)
{
    return *((volatile unsigned int*)address);
}

unsigned int uart_check ( void )
{
    if(read32(AUX_MU_LSR_REG)&0x01) return(1);
    return(0);
}

void uart_init ( void )
{
    unsigned int ra;

    write32(AUX_ENABLES,1);
    write32(AUX_MU_IER_REG,0);
    write32(AUX_MU_CNTL_REG,0);
    write32(AUX_MU_LCR_REG,3);
    write32(AUX_MU_MCR_REG,0);
    write32(AUX_MU_IER_REG,0);
    write32(AUX_MU_IIR_REG,0xC6);
    write32(AUX_MU_BAUD_REG,270);
    ra=read32(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    ra&=~(7<<15); //gpio15
    ra|=2<<15;    //alt5
    write32(GPFSEL1,ra);
    write32(GPPUD,0);
    //for(ra=0;ra<150;ra++) dummy(ra);
    write32(GPPUDCLK0,(1<<14)|(1<<15));
    //for(ra=0;ra<150;ra++) dummy(ra);
    write32(GPPUDCLK0,0);
    write32(AUX_MU_CNTL_REG,3);

    uartMutex = xSemaphoreCreateMutex();
    if(uartMutex == NULL)
    {
        print("Uart mutex hasn't been created successfully, print statements will not be synchronized");
    }
}

void uart_send ( unsigned int c )
{
    while(1)
    {
        if(read32(AUX_MU_LSR_REG)&0x20) break;
    }
    write32(AUX_MU_IO_REG,c);
}

void print(const char * ch) {
  bool printed = false;
  if (uartMutex != NULL) {
    if (xSemaphoreTake(uartMutex, portMAX_DELAY) == pdTRUE) {
      while ( * ch != 0) {
        uart_send((unsigned int) * ch);
        ch++;
      }
      uart_send(0x0D);
      uart_send(0x0A);
      printed = true;
      xSemaphoreGive(uartMutex);

    }
  }
  if (!printed) // print without locking
  {
    while ( * ch != 0) {
      uart_send((unsigned int) * ch);
      ch++;
    }
    uart_send(0x0D);
    uart_send(0x0A);
  }
}

char temp_string[50];

void clear_str()
{
  for(int i = 0; i < 50; i++)
  {
    temp_string[i] = 0;
  }
}

void print_i(int i)
{
    clear_str();
    itoa(i, temp_string, 10);
    print(temp_string);
}

void print_d(double d)
{
    clear_str();
    ftoa(d, temp_string, 6); // guess precision from double value
    print(temp_string);
}