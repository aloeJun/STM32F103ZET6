#ifndef _ESP8266_H
#define _ESP8266_H

#include "stm32f10x.h"

#define WIFI_ENABLE			GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define WIFI_DISABLE		GPIO_ResetBits(GPIOE,GPIO_Pin_6)
#define MY_SSID					"aloeJun"//���������ĺͷ��ţ��ȵ������2.4GƵ��
#define MY_PASSWORD			"12345679"//���������ĺͷ���
#define MY_IP						"www.bemfa.com"
#define MY_PORT					8344
#define PRODUCTID   "c4981dd4a32b442fbdc944918caa242f"  //�޸�Ϊ�Լ���UID
typedef struct
{
	u8 RX_buff[1032];//������յ�������
	u16 RX_count;//��¼�������ݵ�����
	vu8 USART_RevOverflag;//���ս����ı�־
}USART3_DataStruct;
extern USART3_DataStruct USART3_Data;


void usart3_config(void);
void USART3_SendData(u8 data);
void USART3_SendStr(char * p);
void USART3_SendArray(u8 Arr[] ,u16 DataLenth);
char * FindStr(char* dest, char* src, uint32_t timeout);
void Exit_Scotch(void);
uint8_t ESP8266_SendCmd_RecAck(char *cmd, char *ack, uint32_t timeout, uint8_t check_cnt);
u8 Connect_AP(char *SSID,char *PASSWORD);
u8 Connect_Server(char *IP,u16 PORT);//���ӷ�����
void WIFI_Iinit(void);//��ʼ��

#endif
