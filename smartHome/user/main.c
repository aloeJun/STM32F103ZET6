#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bg.h"
#include "dht11.h"
#include "adc.h"
#include "hum.h"
#include "temp.h"
#include "smoke.h"
#include "light.h"
#include "heart.h"
#include "my1680.h"
#include "esp8266.h"
#include "string.h"
#include "xinlv.h"

char buff1[100]={0};

int main(void)
{
	SysTick_Config(72000);
	
	Led_Config(); // LED初始化
	Key_Config(); // KEY初始化
	Usart_Config(); // 串口初始化
	TFTLCD_Init(); // 屏幕初始化
	Dht_Config(DHT_OUT); // DHT(温湿度)模块初始化 配置为输出模式
	ADC_Config(); // ADC(光照、烟雾)模块初始化
	MY1680_Init(); // My1680(音频)模块初始化
	WIFI_Iinit(); // Wifi模块初始化
	Heart_Config(); // 心率模块初始化

	// printf("Hello World"); // USART串口输出内容

	//x,y,width,height,color --> x,y:起点坐标  width,height:区域大小(给定width*height区域用于显示)
	LCD_Fill(0,0,240,320,WHITE); // 画布长宽（0，0，240，320）  画笔颜色 WHITE
	// 显示字符串 x,y,width,height,size,*p,mode --> size:字体大小  12/16/24   *p:字符串起始地址<汉字前可加(u8*)消除警告>   mode:叠加方式(1)还是非叠加方式(0)
	LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // 背景图片
 
	LCD_ShowStringAndHz(50,20,150,16,16,(u8*)"你好世界",0); // 起点坐标(10,20)  区域大小(150,16) 字体大小16
	
	LCD_ShowPicture(0,40,16,16,(u8*)gImage_hum); // 湿度图标
	LCD_ShowPicture(0,60,16,16,(u8*)gImage_temp); // 温度图标
	LCD_ShowPicture(0,80,16,16,(u8*)gImage_smoke); // 烟雾图标
	LCD_ShowPicture(0,100,16,16,(u8*)gImage_light); // 光照图标
	LCD_ShowPicture(0,120,16,16,(u8*)gImage_heart); // 心率图标

  // PlayMP3_42(66,003); // 添加音频 (文件夹名,文件名)
	
	Connect_Server(MY_IP,MY_PORT); // 配置连接服务
	sprintf(buff1,"cmd=1&uid=%s&topic=Tem,Hum,Smoke,Light,Heart\r\n",PRODUCTID);
	USART3_SendArray((u8*)buff1,strlen(buff1));

	
	// KEY控制LED
	uint8_t keyflag = 0; // 获取引脚变量
	while(1)
  {

		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY引脚值
			switch(keyflag)
			{
				case 1: LED1_TOGGLE();PlayMP3_Tem();break; // LED1闪烁 & 播报当前温度
				case 2: LED2_TOGGLE();PlayMP3_Hum();break; // LED2闪烁 & 播报当前湿度
				case 3: LED3_TOGGLE();PlayMP3_Smoke();break; // LED3闪烁 & 播报当前烟雾浓度
				case 4: LED4_TOGGLE();PlayMP3_Light();break; // LED4闪烁 & 播报当前光照强度
			}
			KeyTime=0;
		}

		
		if(DhtTime>1000)
		{ // 获取温湿度的值
			GET_DHT11DATA(); // 输出温湿度的值到LCD

			sprintf(buff1,"cmd=2&uid=%s&topic=Tem&msg=#%.1f#\r\n",PRODUCTID,Tem); // 拼接温度主题
			USART3_SendArray((u8*)buff1,strlen(buff1)); // 上传温度浓度
			
			sprintf(buff1,"cmd=2&uid=%s&topic=Hum&msg=#%.1f#\r\n",PRODUCTID,Hum); // 拼接湿度主题
			USART3_SendArray((u8*)buff1,strlen(buff1)); // 上传湿度浓度

			DhtTime = 0;
		}
		
		if(AdcTime>1000)
		{ // 获取烟雾浓度、光照强度的值
			Get_SmokeValue(); // 输出烟雾浓度的值到LCD
			sprintf(buff1,"cmd=2&uid=%s&topic=Smoke&msg=#%d#\r\n",PRODUCTID,adc_data[1]); // 拼接烟雾主题
			USART3_SendArray((u8*)buff1,strlen(buff1)); // 上传烟雾浓度
			
			Get_LightValue(); // 输出光照强度的值到LCD
			sprintf(buff1,"cmd=2&uid=%s&topic=Light&msg=#%d#\r\n",PRODUCTID,adc_data[0]); // 拼接光照主题
			USART3_SendArray((u8*)buff1,strlen(buff1)); // 上传光照强度

			AdcTime = 0;
		}
		
		
		if(HeartTime > 20)
		{ // 心率模块的获取
			get_BMP();
			sprintf(buff1,"cmd=2&uid=%s&topic=Heart&msg=#%3d#\r\n",PRODUCTID,BPM); // 拼接心率主题
			USART3_SendArray((u8*)buff1,strlen(buff1)); // 上传心率
			
			HeartTime = 0;
		}
	}
}
