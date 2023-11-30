#ifndef _MY1680_H
#define _MY1680_H

#include "stm32f10x.h"


void MY1680_Init(void);
void USART4_SendData(u8 data);
void USART4_SendStr(char * p);
void USART4_SendArray(u8 Arr[] ,u16 DataLenth);
void PlayMP3_41(u8 H,u8 L);
void PlayMP3_42(u8 DIR,u8 L);
void PlayMP3_T(int t);
void PlayMP3_SetVol(u8 v);
void PlayMP3_Weather1(void);
void PlayMP3_Weather2(void);
void PlayMP3_Weather3(void);

#endif
