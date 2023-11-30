#include "usart1.h"



//初始化串口 TXD:PA9 RXD:PA10
void Usart_Config(void)
{
		// 1. 开启时钟
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE); // APB2 外设时钟 GPIOA GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	// 2. 设置工作模式
	GPIO_InitTypeDef usart = {0};
	usart.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推完输出
	usart.GPIO_Pin = GPIO_Pin_9; // 设置GPIO引脚 4、5、6 
	usart.GPIO_Speed = GPIO_Speed_50MHz; // 设置速度
	GPIO_Init(GPIOA,&usart); // 初始化GPIO引脚9
	
	usart.GPIO_Pin = GPIO_Pin_10; // 初始化GPIO引脚10
	usart.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 通用浮空输入
	GPIO_Init(GPIOA,&usart);
	
	USART_InitTypeDef UA = {0};
	UA.USART_BaudRate = 115200; // 波特率
	UA.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流 
	UA.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; // 串口模式 接收|发送
	UA.USART_Parity = USART_Parity_No; // 奇偶校验位
	UA.USART_StopBits = USART_StopBits_1; // 停止位
	UA.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&UA); // 初始化串口
	USART_Cmd(USART1,ENABLE);
	

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


