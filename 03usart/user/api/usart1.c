#include "usart1.h"



//��ʼ������ TXD:PA9 RXD:PA10
void Usart_Config(void)
{
		// 1. ����ʱ��
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE); // APB2 ����ʱ�� GPIOA GPIOC
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	// 2. ���ù���ģʽ
	GPIO_InitTypeDef usart = {0};
	usart.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
	usart.GPIO_Pin = GPIO_Pin_9; // ����GPIO���� 4��5��6 
	usart.GPIO_Speed = GPIO_Speed_50MHz; // �����ٶ�
	GPIO_Init(GPIOA,&usart); // ��ʼ��GPIO����9
	
	usart.GPIO_Pin = GPIO_Pin_10; // ��ʼ��GPIO����10
	usart.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ͨ�ø�������
	GPIO_Init(GPIOA,&usart);
	
	USART_InitTypeDef UA = {0};
	UA.USART_BaudRate = 115200; // ������
	UA.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Ӳ���� 
	UA.USART_Mode = USART_Mode_Rx|USART_Mode_Tx; // ����ģʽ ����|����
	UA.USART_Parity = USART_Parity_No; // ��żУ��λ
	UA.USART_StopBits = USART_StopBits_1; // ֹͣλ
	UA.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&UA); // ��ʼ������
	USART_Cmd(USART1,ENABLE);
	

	//�����ж�
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//�򿪽�������ж�
	//�����жϵ����ȼ�
	NVIC_SetPriority(USART1_IRQn,0);
	//ʹ���ж�
	NVIC_EnableIRQ(USART1_IRQn);
}

//USRT1���͵����ֽڵĺ���
//������Ҫ���͵��ֽ�
void Usart_SendData(uint8_t data)
{
	//�ж��ϴε�������û�з������
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET)
	{}
	//�����µ�����
	USART_SendData(USART1,data);
}

//USART1���ܵ����ֽڵĺ�������������
uint8_t Usart_RecvData(void)
{
	//�ж�������û�н������
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET)
	{}
	//�����յ�������
	return USART_ReceiveData(USART1);
}

//printf���ض���
//printfԭ���Ĺ��ܣ�����Ļ�������������Ҫ�����Ĺ����ض���Ϊ�򴮿�1���

#include "stdio.h"
int fputc(int c,FILE *Stream)
{
	Usart_SendData(c);
	return c;
}

void USART1_IRQHandler(void)
{
	
}


