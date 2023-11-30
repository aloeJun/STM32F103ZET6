#include "stm32f10x.h"
#include "delay.h"
#include "led.h"

int main(void)
{
	Led_Config();
	while(1)
  {
		LED1(1);
		LED2(0);
		Delay_ms(500);
		LED1(0);
		LED2(1);
		Delay_ms(500);
	}
}
