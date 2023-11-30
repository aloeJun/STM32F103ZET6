/*
* 227042420 卢慧军
*/
#include "led.h"

void Led_Config(void)
{
  
	// 1. 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE); // APB2 外设时钟
	
	// 2. 设置工作模式
	GPIO_InitTypeDef led = {0}; // GPIO命名
	led.GPIO_Mode = GPIO_Mode_Out_PP; // 通用推挽输出
	led.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5; // 同时配置GPIO引脚 2、3、4、5
	led.GPIO_Speed = GPIO_Speed_50MHz; // 设置速度
	
	GPIO_Init(GPIOE,&led); // 初始化GPIO
	
	// 关闭LED灯
	LED1(0);
	LED2(0);
	LED3(0);
	LED4(0);
	
}
