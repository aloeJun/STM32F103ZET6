#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

//ø’”Ôæ‰   72MHZ  72000  72
#define delay_1us()	{\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();\
}

extern uint32_t LedTime;
extern uint32_t KeyTime;
extern uint32_t DhtTime;
extern uint32_t AdcTime;
extern uint32_t HeartTime;



void Delay_us(uint32_t time);
void Delay_ms(uint32_t time);
void Delay_nus(unsigned int t);
void Delay_nms(unsigned int t);

#endif


