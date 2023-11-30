/*
* 227042420 卢慧军
*/
#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

#define LED1(x) (x)?(GPIO_ResetBits(GPIOE,GPIO_Pin_2)):(GPIO_SetBits(GPIOE,GPIO_Pin_2))
#define LED2(x) (x)?(GPIO_ResetBits(GPIOE,GPIO_Pin_3)):(GPIO_SetBits(GPIOE,GPIO_Pin_3))
#define LED3(x) (x)?(GPIO_ResetBits(GPIOE,GPIO_Pin_4)):(GPIO_SetBits(GPIOE,GPIO_Pin_4))
#define LED4(x) (x)?(GPIO_ResetBits(GPIOE,GPIO_Pin_5)):(GPIO_SetBits(GPIOE,GPIO_Pin_5))
#define LED1_TOGGLE()  (GPIOE->ODR ^= (0x1<<2))
#define LED2_TOGGLE()  (GPIOE->ODR ^= (0x1<<3))
#define LED3_TOGGLE()  (GPIOE->ODR ^= (0x1<<4))
#define LED4_TOGGLE()  (GPIOE->ODR ^= (0x1<<5))

void Led_Config(void);

#endif
