#include "esp8266.h"
#include "stdio.h"
#include "delay.h"
#include "string.h"
#include "stdlib.h"

USART3_DataStruct USART3_Data={0};

//PB10:TXD   	PB11:RXD
void usart3_config(void)
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	//��IO��ģʽ
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF_PP;//��������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//�������
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	//��usart�Ĺ���ģʽ
	USART_InitTypeDef USART3_InitStruct;
	USART3_InitStruct.USART_BaudRate=115200;//������
	USART3_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//Ӳ������ʧ��
	USART3_InitStruct.USART_WordLength=USART_WordLength_8b;//���ݳ���
	USART3_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//����  ����
	USART3_InitStruct.USART_StopBits=USART_StopBits_1;//1λֹͣλ
	USART3_InitStruct.USART_Parity=USART_Parity_No;//����żУ��
	USART_Init(USART3,&USART3_InitStruct);
	USART_Cmd(USART3,ENABLE);//ʹ�ܴ���3
	
	//�����ж�����
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	//NVIC����
	NVIC_EnableIRQ(USART3_IRQn);
	NVIC_SetPriority(USART3_IRQn,1);
	
	WIFI_DISABLE;
	Delay_ms(50);
	WIFI_ENABLE;
}

void WIFI_Iinit(void)
{
	usart3_config();
	Delay_ms(2000);
	Connect_AP(MY_SSID,MY_PASSWORD);
}

void USART3_SendData(u8 data)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET)
	{}//�ȴ���һ֡���ݷ������
	USART_SendData(USART3,data);//��������
}

void USART3_SendStr(char * p)
{
	while(*p != '\0')
	{
		USART3_SendData(*p);
		p++;
	}
}

void USART3_SendArray(u8 Arr[] ,u16 DataLenth)
{
	u16 i=0;
	while(DataLenth --)
	{
		USART3_SendData(Arr[i++]);
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		u8 data = USART3->DR;
		USART1->DR = data;
		USART3_Data.RX_buff[USART3_Data.RX_count]=USART3->DR;
		USART3_Data.RX_count++;
		if(USART3_Data.RX_count > 1032)
			USART3_Data.RX_count=0;
	}
	if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)
	{
		u8 flag=0;
		flag=USART3->SR;
		flag=USART3->DR;//���ж�
		USART3_Data.USART_RevOverflag=1;//�������
	}
}

/************************************
�ַ�������
����1�����ĸ��ַ����в���
����2��Ҫ���ҵ�����
����3����ʱʱ��
����ֵ�����ҽ��
*************************************/
char * FindStr(char* dest, char* src, uint32_t timeout)
{
	while(timeout-- && (strstr(dest, src) == NULL))
	{
		Delay_ms(1);
	}
	return strstr(dest, src);
}

/**
 * ���ܣ�ESP8266����ָ���ȡӦ��
 * ������
 *       cmd -- ָ���ַ���
 *       ack -- Ӧ���ַ���
 *       timeout -- Ӧ�����ʱ��
 *			 check_cnt -- ѭ������ָ��Ĵ���������ģ��û��Ӧ�������෢���Σ�
 * ����ֵ��0 -- �ɹ�		>0 -- ʧ��
 */

uint8_t ESP8266_SendCmd_RecAck(char *cmd, char *ack, uint32_t timeout, uint8_t check_cnt)
{
	uint32_t len = strlen((char *)cmd);
	USART3_Data.RX_count = 0;
	memset(USART3_Data.RX_buff, 0, sizeof(USART3_Data.RX_buff));

	do{
		//��������
		if(cmd != NULL)
		{
			USART3_SendArray((uint8_t *)cmd, len);
			USART3_SendStr("\r\n");
		}
		//��ѯӦ���ַ���
		if(ack != NULL)
		{
			if(FindStr((char *)USART3_Data.RX_buff, ack, timeout) != NULL) //�ȴ����ڽ���Ӧ�𣬲�����
			{
				USART3_Data.RX_count = 0;
				return 0;//���յ���ȷӦ��
			}
		}
	}while(--check_cnt);
	return 1;//ʧ��
}

//�˳�͸��ģʽ
void Exit_Scotch(void)
{
		USART3_SendStr("+++");
		Delay_ms(500);
}

/************************************
STAģʽ ����2.4GHz���ȵ�
����1��SSID���ȵ���
����2��PASSWORD���ȵ�����
����ֵ��
*************************************/
u8 Connect_AP(char *SSID,char *PASSWORD)
{
	char sendarr[256]={0};
	Exit_Scotch();//�˳���͸ģʽ
	if(ESP8266_SendCmd_RecAck("AT","OK",500,2))
	{
		printf("AT ����ʧ��\r\n");
		return 1;
	}
	else
		printf("AT ���Գɹ�\r\n");
	if(ESP8266_SendCmd_RecAck("AT+CWMODE=1","OK",500,2))
	{
		printf("AT+CWMODE ����ʧ��\r\n");
		return 2;
	}
	else
		printf("AT+CWMODE ���óɹ�\r\n");
	sprintf(sendarr,"AT+CWJAP=\"%s\",\"%s\"",SSID,PASSWORD);
	if(ESP8266_SendCmd_RecAck(sendarr,"OK",10000,2))
	{
		printf("AT+CWJAP ����ʧ��\r\n");
		return 3;
	}
	else
	{
		printf("AT+CWJAP ���óɹ�\r\n");
		printf("�����ȵ�ɹ�\r\n");
	}
	return 0;
}

/************************************
���ӷ�����
����1��IP��������������IP��ַ
����2��PORT���������˿ں�
����ֵ��
*************************************/
u8 Connect_Server(char *IP,u16 PORT)
{
	char sendarr[256]={0};
	if(ESP8266_SendCmd_RecAck("AT+CWJAP?","OK",5000,2) == 1)
	{
		printf("δ��⵽�����ȵ�\r\n������...\r\n");
		if(Connect_AP(MY_SSID,MY_PASSWORD))
		{
			printf("δ��⵽�ȵ㣬�˳�\r\n");
			return 1;
		}
	}
	sprintf(sendarr,"AT+CIPSTART=\"TCP\",\"%s\",%d",IP,PORT);
	if(ESP8266_SendCmd_RecAck(sendarr,"OK",2000,2) == 1)
	{
		printf("���ӷ�����ʧ�ܣ��˳�\r\n");
		return 2;
	}
	if(ESP8266_SendCmd_RecAck("AT+CIPMODE=1","OK",500,2) == 1)
	{
		printf("����͸��ģʽʧ�ܣ��˳�\r\n");
		return 3;
	}
	if(ESP8266_SendCmd_RecAck("AT+CIPSEND",">",500,2) == 1)
	{
		printf("����͸��ģʽʧ�ܣ��˳�\r\n");
		return 4;
	}
	return 0;
}


//�������ϴ�ָ��
//sprintf(buff1,"cmd=1&uid=%s&topic=Tem,Hum,heart,Step,Smoke,Light\r\n",PRODUCTID);			
//	USART3_SendArray((u8*)buff1,strlen(buff1));


//�ϴ�ָ��
//sprintf(buff1,"cmd=2&uid=%s&topic=Tem&msg=#%.1f#\r\n",PRODUCTID,Tem);
//USART3_SendArray((u8*)buff1,strlen(buff1));