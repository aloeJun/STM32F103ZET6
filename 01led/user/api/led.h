/*
* 227042420 卢慧军
*/
#ifndef _LED_H_
#define _LED_H_


#include "stm32f10x.h"

#define LED1(x) (x)? (GPIO_ResetBits(GPIOE,GPIO_Pin_2)):(GPIO_SetBits(GPIOE,GPIO_Pin_2))
#define LED2(x) (x)? (GPIO_ResetBits(GPIOE,GPIO_Pin_3)):(GPIO_SetBits(GPIOE,GPIO_Pin_3))

void Led_Config(void);


#endif
