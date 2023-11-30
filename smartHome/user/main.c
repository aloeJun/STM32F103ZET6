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
	
	Led_Config(); // LED��ʼ��
	Key_Config(); // KEY��ʼ��
	Usart_Config(); // ���ڳ�ʼ��
	TFTLCD_Init(); // ��Ļ��ʼ��
	Dht_Config(DHT_OUT); // DHT(��ʪ��)ģ���ʼ�� ����Ϊ���ģʽ
	ADC_Config(); // ADC(���ա�����)ģ���ʼ��
	MY1680_Init(); // My1680(��Ƶ)ģ���ʼ��
	WIFI_Iinit(); // Wifiģ���ʼ��
	Heart_Config(); // ����ģ���ʼ��

	// printf("Hello World"); // USART�����������

	//x,y,width,height,color --> x,y:�������  width,height:�����С(����width*height����������ʾ)
	LCD_Fill(0,0,240,320,WHITE); // ��������0��0��240��320��  ������ɫ WHITE
	// ��ʾ�ַ��� x,y,width,height,size,*p,mode --> size:�����С  12/16/24   *p:�ַ�����ʼ��ַ<����ǰ�ɼ�(u8*)��������>   mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
	LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // ����ͼƬ
 
	LCD_ShowStringAndHz(50,20,150,16,16,(u8*)"�������",0); // �������(10,20)  �����С(150,16) �����С16
	
	LCD_ShowPicture(0,40,16,16,(u8*)gImage_hum); // ʪ��ͼ��
	LCD_ShowPicture(0,60,16,16,(u8*)gImage_temp); // �¶�ͼ��
	LCD_ShowPicture(0,80,16,16,(u8*)gImage_smoke); // ����ͼ��
	LCD_ShowPicture(0,100,16,16,(u8*)gImage_light); // ����ͼ��
	LCD_ShowPicture(0,120,16,16,(u8*)gImage_heart); // ����ͼ��

  // PlayMP3_42(66,003); // �����Ƶ (�ļ�����,�ļ���)
	
	Connect_Server(MY_IP,MY_PORT); // �������ӷ���
	sprintf(buff1,"cmd=1&uid=%s&topic=Tem,Hum,Smoke,Light,Heart\r\n",PRODUCTID);
	USART3_SendArray((u8*)buff1,strlen(buff1));

	
	// KEY����LED
	uint8_t keyflag = 0; // ��ȡ���ű���
	while(1)
  {

		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY����ֵ
			switch(keyflag)
			{
				case 1: LED1_TOGGLE();PlayMP3_Tem();break; // LED1��˸ & ������ǰ�¶�
				case 2: LED2_TOGGLE();PlayMP3_Hum();break; // LED2��˸ & ������ǰʪ��
				case 3: LED3_TOGGLE();PlayMP3_Smoke();break; // LED3��˸ & ������ǰ����Ũ��
				case 4: LED4_TOGGLE();PlayMP3_Light();break; // LED4��˸ & ������ǰ����ǿ��
			}
			KeyTime=0;
		}

		
		if(DhtTime>1000)
		{ // ��ȡ��ʪ�ȵ�ֵ
			GET_DHT11DATA(); // �����ʪ�ȵ�ֵ��LCD

			sprintf(buff1,"cmd=2&uid=%s&topic=Tem&msg=#%.1f#\r\n",PRODUCTID,Tem); // ƴ���¶�����
			USART3_SendArray((u8*)buff1,strlen(buff1)); // �ϴ��¶�Ũ��
			
			sprintf(buff1,"cmd=2&uid=%s&topic=Hum&msg=#%.1f#\r\n",PRODUCTID,Hum); // ƴ��ʪ������
			USART3_SendArray((u8*)buff1,strlen(buff1)); // �ϴ�ʪ��Ũ��

			DhtTime = 0;
		}
		
		if(AdcTime>1000)
		{ // ��ȡ����Ũ�ȡ�����ǿ�ȵ�ֵ
			Get_SmokeValue(); // �������Ũ�ȵ�ֵ��LCD
			sprintf(buff1,"cmd=2&uid=%s&topic=Smoke&msg=#%d#\r\n",PRODUCTID,adc_data[1]); // ƴ����������
			USART3_SendArray((u8*)buff1,strlen(buff1)); // �ϴ�����Ũ��
			
			Get_LightValue(); // �������ǿ�ȵ�ֵ��LCD
			sprintf(buff1,"cmd=2&uid=%s&topic=Light&msg=#%d#\r\n",PRODUCTID,adc_data[0]); // ƴ�ӹ�������
			USART3_SendArray((u8*)buff1,strlen(buff1)); // �ϴ�����ǿ��

			AdcTime = 0;
		}
		
		
		if(HeartTime > 20)
		{ // ����ģ��Ļ�ȡ
			get_BMP();
			sprintf(buff1,"cmd=2&uid=%s&topic=Heart&msg=#%3d#\r\n",PRODUCTID,BPM); // ƴ����������
			USART3_SendArray((u8*)buff1,strlen(buff1)); // �ϴ�����
			
			HeartTime = 0;
		}
	}
}
