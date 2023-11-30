#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f10x.h"
#define DHT_OUT  0
#define DHT_IN  1
#define DHT11_L  (GPIO_ResetBits(GPIOG,GPIO_Pin_11))
#define DHT11_H  (GPIO_SetBits(GPIOG,GPIO_Pin_11))
#define DHT11_DATA (GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11))


extern float Hum ;//Êª¶È
extern float Tem ;//ÎÂ¶È



void Dht_Config(u8 mode);
u8 GET_DHT11DATA(void);
#endif


