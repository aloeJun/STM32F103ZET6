#include "usart1.h"



//初始化串口 TXD:PA9 RXD:PA10
void Usart_Config(void)
{
	
	
	//配置中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//打开接收完成中断
	//配置中断的优先级
	NVIC_SetPriority(USART1_IRQn,0);
	//使能中断
	NVIC_EnableIRQ(USART1_IRQn);
}

//USRT1发送单个字节的函数
//参数：要发送的字节
void Usart_SendData(uint8_t data)
{
	//判断上次的数据有没有发送完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET)
	{}
	//发送新的数据
	USART_SendData(USART1,data);
}

//USART1接受单个字节的函数，阻塞接收
uint8_t Usart_RecvData(void)
{
	//判断数据有没有接收完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET)
	{}
	//返回收到的数据
	return USART_ReceiveData(USART1);
}

//printf的重定向
//printf原本的功能：向屏幕输出，咱们现在要把它的功能重定向为向串口1输出

#include "stdio.h"
int fputc(int c,FILE *Stream)
{
	Usart_SendData(c);
	return c;
}

void USART1_IRQHandler(void)
{
	
}


