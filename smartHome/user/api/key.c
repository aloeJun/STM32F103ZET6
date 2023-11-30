/*
* 227042420 卢慧军
*/
#include "key.h"
#include "delay.h"

// 初始化Key配置
void Key_Config(void)
{
	// 1. 开启时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE); // APB2 外设时钟 GPIOA GPIOC
	
	// 2. 设置工作模式
	GPIO_InitTypeDef key = {0};
	key.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 通用浮空输入
	key.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6; // 设置GPIO引脚 4、5、6 
	key.GPIO_Speed = GPIO_Speed_50MHz; // 设置速度
	
	GPIO_Init(GPIOC,&key); // 初始化GPIO
	key.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&key);
}

// 获取当前Key的电平
uint8_t Get_Key(void)
{
	static uint16_t count1=0;
	static uint16_t count2=0;
	static uint16_t count3=0;
	static uint16_t count4=0;
	
	if(KEY1==1) count1++;
	else if (KEY2==0) count2++;
	else if (KEY3==0) count3++;
	else if (KEY4==0) count4++;
	else if (count1>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 1;
	}
	else if (count2>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 2;
	}
	else if (count3>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 3;
	}
	else if (count4>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 4;
	}
	return 0;
}

