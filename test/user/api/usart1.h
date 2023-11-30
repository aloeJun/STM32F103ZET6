#ifndef	__USART_H
#define __USART_H

#include "stm32f10x.h"

void Usart_Config(void);
void Usart_SendData(uint8_t data);
uint8_t Usart_RecvData(void);

#endif
