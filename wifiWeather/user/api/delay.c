#include "delay.h"
#include "bsp_lcd.h"
#include "dht11.h"
#include "adc.h"
#include "bg2.h"
#include "hum.h"
#include "temp.h"
#include "smoke.h"
#include "light.h"

#include "my1680.h"


uint32_t LedTime = 0;
uint32_t KeyTime = 0;
uint32_t DhtTime = 0;
uint32_t AdcTime = 0;

void showInfor(void)
{
		Dht_Config(DHT_OUT); // DHT(温湿度)初始化 配置为输出模式
		ADC_Config(); // ADC(光照、烟雾)初始化
		
		LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg2); // 背景图片
		LCD_ShowStringAndHz(80,0,150,16,16,(u8*)"河南工学院",0);
		LCD_ShowStringAndHz(50,20,150,16,16,(u8*)"卢慧军 2207042420",0);
		LCD_ShowPicture(80,40,16,16,(u8*)gImage_hum); // 湿度图标
		LCD_ShowPicture(80,60,16,16,(u8*)gImage_temp); // 温度图标
		LCD_ShowPicture(80,80,16,16,(u8*)gImage_smoke); // 烟雾图标
		LCD_ShowPicture(80,100,16,16,(u8*)gImage_light); // 光照图标
		while(1)
		{
			if(DhtTime>1000)
			{
				GET_DHT11DATA();
				DhtTime = 0;
			}
			
			if(AdcTime>1000)
			{
				Get_SmokeValue();
				Get_LightValue();
				AdcTime = 0;
			}
		}
}

// 定时器
void SysTick_Handler(void)
{
	LedTime ++;
	KeyTime ++;
	DhtTime ++;
	AdcTime ++;
}

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





