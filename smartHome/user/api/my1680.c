#include "my1680.h"
#include "string.h"
#include "esp8266.h"
#include "stdlib.h"
#include "dht11.h"
#include "adc.h"


void MY1680_Init(void)
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	//��IO��ģʽ
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF_PP;//��������
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//��usart�Ĺ���ģʽ
	USART_InitTypeDef USART4_InitStruct;
	USART4_InitStruct.USART_BaudRate=9600;//������
	USART4_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//Ӳ������ʧ��
	USART4_InitStruct.USART_WordLength=USART_WordLength_8b;//���ݳ���
	USART4_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//����  ����
	USART4_InitStruct.USART_StopBits=USART_StopBits_1;//1λֹͣλ
	USART4_InitStruct.USART_Parity=USART_Parity_No;//����żУ��
	USART_Init(UART4,&USART4_InitStruct);
	USART_Cmd(UART4,ENABLE);//ʹ�ܴ���1
}

//���͵����ֽ�
void USART4_SendData(u8 data)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET)
	{}//�ȴ���һ֡���ݷ������
	USART_SendData(UART4,data);//��������
}

//�����ԡ�\0����β���ַ���
void USART4_SendStr(char * p)
{
	while(*p != '\0')
	{
		USART4_SendData(*p);
		p++;
	}
}

//���Ͷ�������
void USART4_SendArray(u8 Arr[] ,u16 DataLenth)
{
	u16 i=0;
	while(DataLenth --)
	{
		USART4_SendData(Arr[i++]);
	}
}

/************************************
����MY1680оƬ��Ŀ¼�еĸ���
����1����Ŀ���Ƹ�λ
����2����Ŀ���Ƶ�λ
��ע����Ŀ����������ʽ0001������.mp3
*************************************/
void PlayMP3_41(u8 H,u8 L)
{
	USART4_SendData(0x7E);
	USART4_SendData(0x05);
	USART4_SendData(0x41);
	USART4_SendData(H);
	USART4_SendData(L);
	u8 cheak = (0x05^0x41^H^L);
	USART4_SendData(cheak);
	USART4_SendData(0xEF);
}

/************************************
����MY1680оƬĿ¼�еĸ���
����1��Ŀ¼����
����2����Ŀ���Ƶ�λ
��ע��Ŀ¼����������ʽ00-99����Ŀ����������ʽ001 ������.mp3
*************************************/
void PlayMP3_42(u8 DIR,u8 L)
{
	USART4_SendData(0x7E);
	USART4_SendData(0x05);
	USART4_SendData(0x42);
	USART4_SendData(DIR);
	USART4_SendData(L);
	u8 cheak = (0x05^0x42^DIR^L);
	USART4_SendData(cheak);
	USART4_SendData(0xEF);
}

/************************************
��������
������Ҫ���ŵ�����(-255~255)
*************************************/
void PlayMP3_T(int t)
{
	u8 i,j,k;
	if(t<0)
	{
		PlayMP3_42(00,24);
		t = -t;
	}
	if(t<20)
	{
		PlayMP3_42(00,t);
		return;
	}
	k=t/100;//�жϰ�λ
	i=(t%100)/10;//�ж�ʮλ
	j=t%10;//�жϸ�λ
	if(k != 0)
	{
			PlayMP3_42(00,k);
			PlayMP3_42(00,20);
	}
	if(i != 0)
	{
		PlayMP3_42(00,i);
		PlayMP3_42(00,10);
	}
	if(j != 0)
	{	
		if(i == 0)
		{
			PlayMP3_42(00,00);
		}
		PlayMP3_42(00,j);
	}
}

//��������(0-30)
void PlayMP3_SetVol(u8 v)
{
	if(v>30)
		v=30;
	USART4_SendData(0x7E);
	USART4_SendData(0x04);
	USART4_SendData(0x31);
	USART4_SendData(v);
	u8 cheak = (0x05^0x42^v);
	USART4_SendData(cheak);
	USART4_SendData(0xEF);
}

//������������
/*void PlayMP3_Weather1(void)
{
	PlayMP3_42(06,00);
	PlayMP3_42(04,04);
	PlayMP3_42(02,03);
	//PlayMP3_42(02,atoi(Weather_Data[0].codeday));//atoi���ַ�Xת��Ϊ����X
	PlayMP3_42(02,02);
	//PlayMP3_42(02,atoi(Weather_Data[0].codenight));
	int i=0;
	PlayMP3_42(04,02);
	//i=atoi(Weather_Data[0].temhigh);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
	PlayMP3_42(04,03);
	//i=atoi(Weather_Data[0].temlow);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
}


//������������
void PlayMP3_Weather2(void)
{
	PlayMP3_42(06,00);
	PlayMP3_42(04,05);
	PlayMP3_42(02,03);
	PlayMP3_42(02,atoi(Weather_Data[1].codeday));
	PlayMP3_42(02,02);
	PlayMP3_42(02,atoi(Weather_Data[1].codenight));
	int i=0;
	PlayMP3_42(04,02);
	i=atoi(Weather_Data[1].temhigh);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
	PlayMP3_42(04,03);
	i=atoi(Weather_Data[1].temlow);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
}

//������������
void PlayMP3_Weather3(void)
{
	PlayMP3_42(06,00);
	PlayMP3_42(04,06);
	PlayMP3_42(02,03);
	PlayMP3_42(02,atoi(Weather_Data[2].codeday));
	PlayMP3_42(02,02);
	PlayMP3_42(02,atoi(Weather_Data[2].codenight));
	int i=0;
	PlayMP3_42(04,02);
	i=atoi(Weather_Data[2].temhigh);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
	PlayMP3_42(04,03);
	i=atoi(Weather_Data[2].temlow);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
}*/
//������ǰ�¶�
void PlayMP3_Tem(void)
{
	PlayMP3_42(01,05); // ��ǰ�����¶�
	PlayMP3_T(Tem); // �¶�ֵ
	PlayMP3_42(01,01); // ���϶�
}
//������ǰʪ��
void PlayMP3_Hum(void)
{
	PlayMP3_42(01,06); // ��ǰ����ʪ��
	PlayMP3_42(01,02); // �ٷ�֮
	PlayMP3_T(Hum); // ʪ��ֵ
}
//������ǰ����ǿ��
void PlayMP3_Light(void)
{ 
	PlayMP3_42(01,07); // ��ǰ���ڹ���ǿ��
	PlayMP3_T(adc_data[0]/1000); // ����ǿ��ǧλ��ֵ
	PlayMP3_42(00,21); // ǧ
	
	if((adc_data[0]/100)%10==0)
	{
		PlayMP3_42(00,000);
	}
	
	PlayMP3_T(adc_data[0]%1000);
	PlayMP3_42(01,03); // LUX
}
//������ǰ����Ũ��
void PlayMP3_Smoke(void)
{
	PlayMP3_42(01,8); // ��ǰ��������Ũ��
	PlayMP3_T(adc_data[1]); // ����Ũ��ֵ
	PlayMP3_42(01,04); // PPM
}
