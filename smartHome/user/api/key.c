/*
* 227042420 ¬�۾�
*/
#include "key.h"
#include "delay.h"

// ��ʼ��Key����
void Key_Config(void)
{
	// 1. ����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE); // APB2 ����ʱ�� GPIOA GPIOC
	
	// 2. ���ù���ģʽ
	GPIO_InitTypeDef key = {0};
	key.GPIO_Mode = GPIO_Mode_IN_FLOATING; // ͨ�ø�������
	key.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6; // ����GPIO���� 4��5��6 
	key.GPIO_Speed = GPIO_Speed_50MHz; // �����ٶ�
	
	GPIO_Init(GPIOC,&key); // ��ʼ��GPIO
	key.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&key);
}

// ��ȡ��ǰKey�ĵ�ƽ
uint8_t Get_Key(void)
{
	static uint16_t count1=0;
	static uint16_t count2=0;
	static uint16_t count3=0;
	static uint16_t count4=0;
	
	if(KEY1==1) count1++;
	else if (KEY2==0) count2++;
	else if (KEY3==0) count3++;
	else if (KEY4==0) count4++;
	else if (count1>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 1;
	}
	else if (count2>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 2;
	}
	else if (count3>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 3;
	}
	else if (count4>2)
	{
		count1=0;
		count2=0;
		count3=0;
		count4=0;
		return 4;
	}
	return 0;
}

