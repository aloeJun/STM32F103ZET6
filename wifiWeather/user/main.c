#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bg.h"
#include "my1680.h"
#include "adc.h"
#include "dht11.h"
#include "esp8266.h"


int main(void)
{
	SysTick_Config(72000);
	
	Led_Config(); // LED初始化
	Key_Config(); // KEY初始化
	Usart_Config(); // 串口初始化
	// printf("Hello World"); // USART串口输出内容

	MY1680_Init(); // My1680(音频)初始化
	PlayMP3_42(66,003); // 添加音频 (文件夹名,文件名)
	
	TFTLCD_Init(); // 屏幕初始化函数
	//x,y,width,height,color --> x,y:起点坐标  width,height:区域大小(给定width*height区域用于显示)
	LCD_Fill(0,0,240,320,WHITE); // 画布长宽（0，0，240，320）  画笔颜色 WHITE
	// 显示字符串 x,y,width,height,size,*p,mode --> size:字体大小  12/16/24   *p:字符串起始地址<汉字前可加(u8*)消除警告>   mode:叠加方式(1)还是非叠加方式(0)
	LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // 背景图片
	LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"你好世界",0); // 起点坐标(50,40)  区域大小(150,16) 字体大小16
	
	
	WIFI_Iinit(); // Wifi初始化
	Connect_Server(MY_IP,MY_PORT); // 配置连接心知天气服务
	
	// KEY控制LED
	uint8_t keyflag = 0; // 获取引脚变量
	while(1)
  {
		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY引脚值
			switch(keyflag)
			{
				case 1: // 开启LED2闪烁 & 播报今日天气
					LED2_TOGGLE();
					LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // 背景图片
					LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"卢慧军 2207042420",0);
					LCD_ShowStringAndHz(90,20,150,16,16,(u8*)"今日天气",0);
					PlayMP3_Weather1();
					break; 
				case 2: // 开启LED3闪烁 & 播报明日天气
					LED3_TOGGLE();
					LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // 背景图片
					LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"卢慧军 2207042420",0);
					LCD_ShowStringAndHz(90,20,150,16,16,(u8*)"明日天气",0);
					PlayMP3_Weather2();
					break; 
				case 3:  // 开启LED4闪烁 & 播报后日天气
					LED4_TOGGLE();
					LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // 背景图片
					LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"卢慧军 2207042420",0);
					LCD_ShowStringAndHz(90,20,150,16,16,(u8*)"后日天气",0);
					PlayMP3_Weather3();
					break;
				case 4: // 开启LED2、3、4闪烁、展示温湿度等信息
					LED2_TOGGLE();
					LED3_TOGGLE();
					LED4_TOGGLE();
					showInfor(); // 展示当前室内温度页面
				break; 
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

