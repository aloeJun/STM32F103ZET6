#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "stdio.h"

int main(void)
{
	SysTick_Config(72000);
	
	Led_Config(); // LED��ʼ��
	Key_Config(); // KEY��ʼ��
	Usart_Config(); // ���ڳ�ʼ��
	printf("Hello World!");

	uint8_t keyflag = 0; // ��ȡ���ű���
	

	
	while(1)
  {

		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY����ֵ
			switch(keyflag)
			{
				case 1: LED3_TOGGLE();break;
				case 2: LED2_TOGGLE();break;
				case 3: LED3_TOGGLE();break;
				case 4: LED4_TOGGLE();break;
			}
			KeyTime=0;
		}

		if(LedTime>=500)
		{
			LED1_TOGGLE();
			LedTime = 0;
		}
	}
}
