#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "rx.h"
#include "dht11.h"
#include "adc.h"


int main(void)
{
	SysTick_Config(72000);

	Led_Config(); // LED初始化
	Key_Config(); // KEY初始化
	Usart_Config(); // 串口初始化
	Dht_Config(DHT_OUT); // 配置为输出模式
	ADC_Config(); // ADC初始化 ==>光照、烟雾
	
	TFTLCD_Init(); // 屏幕初始化函数
	//x,y,width,height,color --> x,y:起点坐标  width,height:区域大小(给定width*height区域用于显示)
	LCD_Fill(0,0,240,320,WHITE); // 画布长宽（0，0，240，320）  画笔颜色 WHITE

	//显示字符串 x,y,width,height,size,*p,mode --> size:字体大小  12/16/24   *p:字符串起始地址<汉字前可加(u8*)消除警告>   mode:叠加方式(1)还是非叠加方式(0)
	
	LCD_ShowPicture(0,0,240,320,(u8*)gImage_rx); // rx.h图片
	
	LCD_ShowStringAndHz(50,20,150,16,16,(u8*)"你好世界",0); // 起点坐标(10,20)  区域大小(150,16) 字体大小16  

	printf("Hello World"); // USART串口输出内容

	uint8_t keyflag = 0; // 获取引脚变量
	
	while(1)
  {

		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY引脚值
			switch(keyflag)
			{
				case 1: LED2_TOGGLE();LED3_TOGGLE();LED4_TOGGLE();break;
				case 2: LED2_TOGGLE();break;
				case 3: LED3_TOGGLE();break;
				case 4: LED4_TOGGLE();
								Dht_Config(DHT_OUT); // 配置为输出模式
								ADC_Config(); // ADC初始化 ==>光照、烟雾
								
									TFTLCD_Init(); // 屏幕初始化函数
								//x,y,width,height,color --> x,y:起点坐标  width,height:区域大小(给定width*height区域用于显示)
								LCD_Fill(0,0,240,320,WHITE); // 画布长宽（0，0，240，320）  画笔颜色 WHITE

								//显示字符串 x,y,width,height,size,*p,mode --> size:字体大小  12/16/24   *p:字符串起始地址<汉字前可加(u8*)消除警告>   mode:叠加方式(1)还是非叠加方式(0)
								LCD_ShowPicture(0,0,240,320,(u8*)gImage_rx); // rx.h图片
				
								LCD_ShowStringAndHz(50,20,150,16,16,(u8*)" 2207042420",0); // 起点坐标(10,20)  区域大小(150,16) 字体大小16  
								
								break;
			}
			KeyTime=0;
		}

		if(LedTime>=500)
		{
			LED1_TOGGLE();
			LedTime = 0;
		}
		
		if(DhtTime>1000){
			GET_DHT11DATA();
			DhtTime = 0;
		}
		
		if(AdcTime>1000){
			Get_SmokeValue();
			Get_LightValue();
			AdcTime = 0;
		}
	}
}


