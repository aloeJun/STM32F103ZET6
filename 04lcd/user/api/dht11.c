#include "dht11.h"
#include "delay.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcd.h"


//dht11 PG11
void Dht_Config(u8 mode)
{
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	//配置GPIO
	GPIO_InitTypeDef dht = {0};
	dht.GPIO_Pin = GPIO_Pin_11;
	dht.GPIO_Speed = GPIO_Speed_50MHz;
	if(mode == DHT_OUT)
	{
		dht.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
	}
	else
	{
		dht.GPIO_Mode = GPIO_Mode_IN_FLOATING; //模拟输入
	}
	GPIO_Init(GPIOG,&dht);
}

float Hum = 0;//湿度
float Tem = 0;//温度
u8 data[5] = {0};//存放接收的40位DHT11数据
u8 dht11_buff[50] = {0};
u8 GET_DHT11DATA(void)
{
	u8 check = 0;
	u8 i = 0;
	u8 count = 0;
	//发送起始信号
	Dht_Config(DHT_OUT);
	DHT11_L;
	Delay_ms(20);
	DHT11_H;
	//接收来自于DHT11的响应信号
	Dht_Config(DHT_IN);
	while(DHT11_DATA ==0)
	{
		count++;
		Delay_us(1);
		if(count>100)
		{
			return 1;
		}
	}
	count = 0;
	while(DHT11_DATA ==1)
	{
		count++;
		Delay_us(1);
		if(count>100)
		{
			return 2;
		}
	}
	//等待响应结束
	count = 0;
	while(DHT11_DATA ==0)
	{
		count++;
		Delay_us(1);
		if(count>100)
		{
			return 3;
		}
	}
	//接收40位数据
	for(i=0;i<40;i++)
	{
		count = 0;
		while(DHT11_DATA ==1)
		{
			count++;
			Delay_us(1);
			if(count>100)
			{
				return 4;
			}
		}
		count = 0;
		while(DHT11_DATA ==0)
		{
			count++;
			Delay_us(1);
			if(count>100)
			{
				return 5;
			}
		}
		Delay_us(30);
		if(DHT11_DATA == 0)
		{
			data[i/8] &= ~(1<<(7-i%8));
		}
		if(DHT11_DATA == 1)
		{
			data[i/8] |= (1<<(7-i%8));
			
		}
	}
		check = data[0]+data[1]+data[2]+data[3];
		if(check == data[4])
		{
			Hum = data[0]+data[1]/10.0;
			Tem = data[2]+data[3]/10.0;
			printf("Hum:%.1f  Tem:%.1f\r\n",Hum,Tem); 
			
			TFTLCD_Init(); // 屏幕初始化函数
		//x,y,width,height,color --> x,y:起点坐标  width,height:区域大小(给定width*height区域用于显示)
			LCD_Fill(0,0,240,320,WHITE); // 画布长宽（0，0，240，320）  画笔颜色 WHITE
			
			char buffer[200];
			
			sprintf(buffer,"Hum:%.1f  Tem:%.1f\r\n",Hum,Tem); // 拼接温湿度
			
	//显示字符串 x,y,width,height,size,*p,mode --> size:字体大小  12/16/24   *p:字符串起始地址<汉字前可加(u8*)消除警告>   mode:叠加方式(1)还是非叠加方式(0)
			// LCD_ShowStringAndHz(10,20,150,16,16,(u8*)buffer,0); // 起点坐标(10,20)  区域大小(150,16) 字体大小16  
		
		}
		return 0;
}











