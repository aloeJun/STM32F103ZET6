#ifndef _ESP8266_H
#define _ESP8266_H

#include "stm32f10x.h"

#define WIFI_ENABLE			GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define WIFI_DISABLE		GPIO_ResetBits(GPIOE,GPIO_Pin_6)
#define MY_SSID					"dlb"//不能有中文和符号，热点必须是2.4G频段
#define MY_PASSWORD			"123456789"//不能有中文和符号
#define MY_IP						"www.bemfa.com"
#define MY_PORT					8344
#define PRODUCTID   "1d438a42585d8c718aca7a29612ce3ef"  //修改为自己的UID
typedef struct
{
	u8 RX_buff[1032];//保存接收到的数据
	u16 RX_count;//记录接收数据的数量
	vu8 USART_RevOverflag;//接收结束的标志
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
u8 Connect_Server(char *IP,u16 PORT);//链接服务器
void WIFI_Iinit(void);//初始化

#endif
