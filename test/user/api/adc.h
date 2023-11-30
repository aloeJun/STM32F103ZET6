#ifndef	__ADC_H
#define __ADC_H

#include "stm32f10x.h"

extern u16 adc_data[2];
extern uint8_t mode;//主函数定义的界面切换标志位

void ADC_Config(void);
u16 Get_LightValue(void);
u16 Get_SmokeValue(void);
#endif
