#include "usart1.h"



//��ʼ������ TXD:PA9 RXD:PA10
void Usart_Config(void)
{
	
	
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


