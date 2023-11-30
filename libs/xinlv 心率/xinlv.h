#ifndef __XINLV_H
#define __XINLV_H

#include "stm32f10x.h"

extern u16 ADC_Data;
extern uint16_t BPM;//心率的值


u16 Get_LightVale(void);
void get_BMP(void);//心率值的获取
uint16_t Get_Array_Max(uint16_t * array, uint32_t size);	
uint16_t Get_Array_Min(uint16_t * array, uint32_t size);
void Heart_Config(void);//心率模块初始化

#endif
