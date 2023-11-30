#include "delay.h"


void Delay_us(uint32_t time)
{
	while(time--)
		delay_1us();
}

void Delay_ms(uint32_t time)
{
	uint64_t ms = time*1000;
	while(ms--)
		delay_1us();
}

//us延时函数
void Delay_nus(unsigned int t)
{
	while(t--) {
		delay_1us();//1us
	}
}

//ms延时函数
void Delay_nms(unsigned int t)
{
	unsigned int T = t*1000;
	while(T--) {
		delay_1us();
	}
}

void SysTick_Handler(void)
{
	
}




