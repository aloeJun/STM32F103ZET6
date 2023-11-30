#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"

int main(void)
{
	SysTick_Config(72000);
	
	Led_Config(); // LED初始化
	Key_Config(); // KEY初始化
	
	uint8_t keyflag = 0; // 获取引脚变量
	

	
	while(1)
  {

		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY引脚值
			switch(keyflag)
			{
				case 1: LED3_TOGGLE();break;
				case 2: LED2_TOGGLE();break;
				case 3: LED3_TOGGLE();break;
				case 4: LED4_TOGGLE();break;
			}
			KeyTime=0;
		}

		if(LedTime>=1000) // 闪烁时间周期 1000以500ms时间间隔闪烁
		{
			LED1_TOGGLE();
			LedTime = 0;
		}
	}
}
