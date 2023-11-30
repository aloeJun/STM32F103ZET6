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
	
	Led_Config(); // LED��ʼ��
	Key_Config(); // KEY��ʼ��
	Usart_Config(); // ���ڳ�ʼ��
	// printf("Hello World"); // USART�����������

	MY1680_Init(); // My1680(��Ƶ)��ʼ��
	PlayMP3_42(66,003); // �����Ƶ (�ļ�����,�ļ���)
	
	TFTLCD_Init(); // ��Ļ��ʼ������
	//x,y,width,height,color --> x,y:�������  width,height:�����С(����width*height����������ʾ)
	LCD_Fill(0,0,240,320,WHITE); // ��������0��0��240��320��  ������ɫ WHITE
	// ��ʾ�ַ��� x,y,width,height,size,*p,mode --> size:�����С  12/16/24   *p:�ַ�����ʼ��ַ<����ǰ�ɼ�(u8*)��������>   mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
	LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // ����ͼƬ
	LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"�������",0); // �������(50,40)  �����С(150,16) �����С16
	
	
	WIFI_Iinit(); // Wifi��ʼ��
	Connect_Server(MY_IP,MY_PORT); // ����������֪��������
	
	// KEY����LED
	uint8_t keyflag = 0; // ��ȡ���ű���
	while(1)
  {
		if(KeyTime>10)
		{	
			keyflag = Get_Key(); // KEY����ֵ
			switch(keyflag)
			{
				case 1: // ����LED2��˸ & ������������
					LED2_TOGGLE();
					LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // ����ͼƬ
					LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"¬�۾� 2207042420",0);
					LCD_ShowStringAndHz(90,20,150,16,16,(u8*)"��������",0);
					PlayMP3_Weather1();
					break; 
				case 2: // ����LED3��˸ & ������������
					LED3_TOGGLE();
					LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // ����ͼƬ
					LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"¬�۾� 2207042420",0);
					LCD_ShowStringAndHz(90,20,150,16,16,(u8*)"��������",0);
					PlayMP3_Weather2();
					break; 
				case 3:  // ����LED4��˸ & ������������
					LED4_TOGGLE();
					LCD_ShowPicture(0,0,240,320,(u8*)gImage_bg); // ����ͼƬ
					LCD_ShowStringAndHz(50,40,150,16,16,(u8*)"¬�۾� 2207042420",0);
					LCD_ShowStringAndHz(90,20,150,16,16,(u8*)"��������",0);
					PlayMP3_Weather3();
					break;
				case 4: // ����LED2��3��4��˸��չʾ��ʪ�ȵ���Ϣ
					LED2_TOGGLE();
					LED3_TOGGLE();
					LED4_TOGGLE();
					showInfor(); // չʾ��ǰ�����¶�ҳ��
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

