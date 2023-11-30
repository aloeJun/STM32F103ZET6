#include "dht11.h"
#include "delay.h"
#include "stdio.h"
#include "bsp_lcd.h"

//dht11 PG11
void Dht_Config(u8 mode)
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	//����GPIO
	GPIO_InitTypeDef dht = {0};
	dht.GPIO_Pin = GPIO_Pin_11;
	dht.GPIO_Speed = GPIO_Speed_50MHz;
	if(mode == DHT_OUT)
	{
		dht.GPIO_Mode = GPIO_Mode_Out_OD; //��©���
	}
	else
	{
		dht.GPIO_Mode = GPIO_Mode_IN_FLOATING; //ģ������
	}
	GPIO_Init(GPIOG,&dht);
}

float Hum = 0;//ʪ��
float Tem = 0;//�¶�
u8 data[5] = {0};//��Ž��յ�40λDHT11����
u8 dht11_buff[50] = {0};
u8 GET_DHT11DATA(void)
{
	u8 check = 0;
	u8 i = 0;
	u8 count = 0;
	//������ʼ�ź�
	Dht_Config(DHT_OUT);
	DHT11_L;
	Delay_ms(20);
	DHT11_H;
	//����������DHT11����Ӧ�ź�
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
	//�ȴ���Ӧ����
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
	//����40λ����
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
			
			sprintf((char*)dht11_buff,"ʪ��:%d",data[0]); // ƴ��ʪ��
			LCD_ShowStringAndHz(95,40,150,16,16,(u8*)dht11_buff,0);

			sprintf((char*)dht11_buff,"�¶�:%d",data[2]); // ƴ���¶�
			LCD_ShowStringAndHz(95,60,150,16,16,(u8*)dht11_buff,0);
		}
		return 0;
}











