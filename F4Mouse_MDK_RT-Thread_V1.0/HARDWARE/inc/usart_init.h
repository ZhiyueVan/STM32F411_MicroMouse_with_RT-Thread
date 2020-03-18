#ifndef __USART_INIT_H
#define __USART_INIT_H

#include <string.h>
#include <rtthread.h>
#include <stm32f4xx.h>

int uart_init(void);
void rt_hw_console_output(const char *str);
char rt_hw_console_getchar(void);

#endif
