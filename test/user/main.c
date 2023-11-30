#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart1.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "lhj.h"
#include "dht11.h"
#include "adc.h"
#include "hum.h"
#include "temp.h"
#include "smoke.h"
#include "light.h"
#include "my1680.h"


int main(void)
{
	SysTick_Config(72000);
	
	Led_Config(); // LED��ʼ��
	Key_Config(); // KEY��ʼ��
	Usart_Config(); // ���ڳ�ʼ��
	Dht_Config(DHT_OUT); // DHT(��ʪ��)��ʼ�� ����Ϊ���ģʽ
	ADC_Config(); // ADC(���ա�����)��ʼ��
	MY1680_Init(); // My1680(��Ƶ)��ʼ��

	
	TFTLCD_Init(); // ��Ļ��ʼ������
	//x,y,width,height,color --> x,y:�������  width,height:�����С(����width*height����������ʾ)
	LCD_Fill(0,0,240,320,WHITE); // ��������0��0��240��320��  ������ɫ WHITE

	// ��ʾ�ַ��� x,y,width,height,size,*p,mode --> size:�����С  12/16/24   *p:�ַ�����ʼ��ַ<����ǰ�ɼ�(u8*)��������>   mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
	// LCD_ShowStringAndHz(10,20,150,16,16,(u8*)"���,����",0); // �������(10,20)  �����С(150,16) �����С16  
	
	LCD_ShowPicture(0,0,240,320,(u8*)gImage_lhj); // ����ͼƬ
	LCD_ShowPicture(0,20,16,16,(u8*)gImage_hum); // ʪ��ͼ��
	LCD_ShowPicture(0,40,16,16,(u8*)gImage_temp); // �¶�ͼ��
	LCD_ShowPicture(0,60,16,16,(u8*)gImage_smoke); // ����ͼ��
	LCD_ShowPicture(0,80,16,16,(u8*)gImage_light); // ����ͼ��

  PlayMP3_42(66,001);
	
	printf("Hello World"); // USART�����������

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
