#ifndef _ESP8266_H
#define _ESP8266_H

#include "stm32f10x.h"

#define WIFI_ENABLE			GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define WIFI_DISABLE		GPIO_ResetBits(GPIOE,GPIO_Pin_6)
#define MY_SSID					"aloeJun"//要连接的热点名称，需要大家根据自己的情况修改
#define MY_PASSWORD			"12345679"//要连接的热点密码，，需要大家根据自己的情况修改
#define MY_KEY					"S_qe0XQxkP-XYxY6K"//心知天气的私钥，也要修改成自己的
#define MY_IP						"api.seniverse.com"
#define MY_PORT					80
typedef struct
{
	u8 RX_buff[1032];//保存接收到的数据
	u16 RX_count;//记录接收数据的数量
	vu8 USART_RevOverflag;//接收结束的标志
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
