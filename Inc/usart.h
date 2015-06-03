#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void MX_USART2_UART_Init(void);
void USART2_Reconfig(uint8_t *settings);
size_t USART_ReadData(char *data, size_t maxSize);
bool USART_GetChar(char *c);
size_t USART_WriteData(const void *data, size_t dataSize);
bool USART_PutChar(char c);


#endif