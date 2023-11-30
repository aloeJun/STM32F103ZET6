/*
* 227042420 卢慧军
*/
#ifndef _KEY_H_
#define _KEY_H_


#include "stm32f10x.h"



#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0) // 读取引脚输入电平
#define KEY2 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
#define KEY3 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5)
#define KEY4 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)

void Key_Config(void);

uint8_t Get_Key(void);

#endif
