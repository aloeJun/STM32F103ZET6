#ifndef _ESP8266_H
#define _ESP8266_H

#include "stm32f10x.h"

#define WIFI_ENABLE			GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define WIFI_DISABLE		GPIO_ResetBits(GPIOE,GPIO_Pin_6)
#define MY_SSID					"aloeJun"//Ҫ���ӵ��ȵ����ƣ���Ҫ��Ҹ����Լ�������޸�
#define MY_PASSWORD			"12345679"//Ҫ���ӵ��ȵ����룬����Ҫ��Ҹ����Լ�������޸�
#define MY_KEY					"S_qe0XQxkP-XYxY6K"//��֪������˽Կ��ҲҪ�޸ĳ��Լ���
#define MY_IP						"api.seniverse.com"
#define MY_PORT					80
typedef struct
{
	u8 RX_buff[1032];//������յ�������
	u16 RX_count;//��¼�������ݵ�����
	vu8 USART_RevOverflag;//���ս����ı�־
}USART3_DataStruct;
extern USART3_DataStruct USART3_Data;

typedef struct
{
	char data[20];
	char name[30];
	char weatherday[20];
	char weathernight[20];
	char codeday[10];
	char codenight[10];
	char temhigh[10];
	char temlow[10];
	char humidity[10];
}Weather_DataStruct;

extern Weather_DataStruct Weather_Data[3];

void usart3_config(void);
void USART3_SendData(u8 data);
void USART3_SendStr(char * p);
void USART3_SendArray(u8 Arr[] ,u16 DataLenth);
char * FindStr(char* dest, char* src, uint32_t timeout);
void Exit_Scotch(void);
uint8_t ESP8266_SendCmd_RecAck(char *cmd, char *ack, uint32_t timeout, uint8_t check_cnt);
u8 Connect_AP(char *SSID,char *PASSWORD);
u8 Connect_Server(char *IP,u16 PORT);
void WIFI_Iinit(void);

#endif
