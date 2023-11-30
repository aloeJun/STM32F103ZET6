#include "xinlv.h"
#include "stdio.h"
#include "bsp_lcd.h"

char BMP_show[50] = {0};

uint16_t readData, preReadData;		   		// ��ȡ���� AD ֵ
uint16_t timeCount = 0;         // �������ڼ�������
uint16_t firstTimeCount = 0;    // ��һ������ʱ��
uint16_t secondTimeCount = 0;   // �ڶ�������ʱ��

typedef enum {FALSE, TRUE} BOOL;

/* ���͸���λ���������� IBI: ��������������ʱ�䣬BPM: ����ֵ�� SIG: ����ͼ����ֵ����ʾ */
uint16_t IBI, BPM, SIG;   		

uint16_t data1[50] = {0}; // �������ݻ���
int a = 0; 				// ���������±�
uint16_t max, min, mid;			// �����С���м�ֵ
uint16_t filter;				// �˲�ֵ

BOOL PULSE = FALSE;				// ��Ч����
BOOL PRE_PULSE = FALSE;         // ��ǰ��Ч����

uint8_t pulseCount = 0;			// ��������


//PA1 ADC1_IN1 ����ģ��
void Heart_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_InitTypeDef ADC_InitStruct = {0};
	//����ADC��ʱ��Ϊ12MHZ(������14MHZ)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//��ʱ��	GPIOA		ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	
	//����GPIO�� PA1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;//ģ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//ѡ������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//GPIO�ڵ��������
	//��ʼ��GPIO�� GPIOA
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	
	//����ADC
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//��ʹ������ת��
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ʹ��Ӳ������
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//��������ģʽ
	ADC_InitStruct.ADC_NbrOfChannel = 1;//��ת��ͨ������
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//��ʹ��ɨ��ģʽ
	//��ʼ��ADC
	ADC_Init(ADC1, &ADC_InitStruct);
	//ʹ��ADC
	ADC_Cmd(ADC1,ENABLE);
	
	//����У׼�Ĵ���
	ADC_ResetCalibration(ADC1);
	//�ȴ�У׼�Ĵ����������
	while(ADC_GetResetCalibrationStatus(ADC1));
	//У׼ADC
	ADC_StartCalibration(ADC1);
	//�ȴ�ADCУ׼���
	while(ADC_GetCalibrationStatus(ADC1));
}




u16 Get_LightVale(void)
{
	u8 i,j;
	u16 buff[10] = {0};
	u16 temp;
	
	//����ͨ��	������	ת��˳��	ת��ʱ��
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);//ת��ʱ��Խ�������ݾ�Խ׼ȷ
	
	//����ת��ʮ��
	for(i = 0; i < 10; i ++)
	{
		//������ƿ�ʼת��
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		//�ȴ�ת�����
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == 0);
		//��ȡת������
		buff[i] = ADC_GetConversionValue(ADC1);
	}
	
	//�Ѷ�ȡֵ�����ݰ���С��������У�ð������
	for(i = 0; i < 9; i++)
	{
		for(j = 0 ; j < 9-i; j++)
		{
			if(buff[j] > buff[j+1])
			{
				temp = buff[j];
				buff[j] = buff[j+1];
				buff[j+1] = temp;
			}
		}
	}
	
	//ȥ��һ�����ֵ��ȥ��һ����Сֵ��ƽ��ֵ
	temp = 0;
	for(i = 1; i < 9; i++)
	{
		temp += buff[i];
	}
	
	return temp/8;
}

void get_BMP(void)
{
		preReadData = readData;	        // ����ǰһ��ֵ
		readData = Get_LightVale();		// ��ȡADת��ֵ
		
		if ((readData - preReadData) < filter)    // �˳�ͻ�������źŸ���
			data1[a++] = readData;	// ��仺������

		if (a >= 50)
		{	
			a = 0;	// ������������ͷ����
		
			// ͨ�����������ȡ�����źŵĲ��塢����ֵ���������м�ֵ��Ϊ�ж��ο���ֵ
			max = Get_Array_Max(data1, 50);
			min = Get_Array_Min(data1, 50);
			mid = (max + min)/2;
			filter = (max - min) / 2;
		}
		
		PRE_PULSE = PULSE;	// ���浱ǰ����״̬
		PULSE = (readData > mid) ? TRUE : FALSE;  // ����ֵ�����м�ֵΪ��Ч����
		
		if (PRE_PULSE == FALSE && PULSE == TRUE)  // Ѱ�ҵ����ź�����������м�λ�á��������㣬��⵽һ����Ч����
		{	
			pulseCount++;
			pulseCount %= 2;	 
			
			if(pulseCount == 1) // ���������ĵ�һ��
			{                         	
				firstTimeCount = timeCount;   // ��¼��һ������ʱ��
			}
			if(pulseCount == 0)  // ���������ĵڶ���
			{                             			
				secondTimeCount = timeCount;  // ��¼�ڶ�������ʱ��
				timeCount = 0;	

				if ( (secondTimeCount > firstTimeCount))
				{
					IBI = (secondTimeCount - firstTimeCount) * 20;	// ������������������ʱ�䣬�õ� IBI
					BPM = 60000 / IBI;  // ͨ�� IBI �õ�����ֵ BPM
					
					if (BPM > 200)    //����BPM�����ʾֵ
						BPM = 200;	 				
					if (BPM < 30)    //����BPM�����ʾֵ
						BPM=30;
				}
			
			}
				printf("BMP:%3d\r\n",BPM);//����ֵ���ݴ�ӡ
				sprintf(BMP_show,"BMP:%3d",BPM);
				LCD_ShowStringAndHz(15,120,80,16,16,(u8*)BMP_show,0);
				
		}

		timeCount++;  // ʱ������ۼ�
}

uint16_t Get_Array_Max(uint16_t * array, uint32_t size)
{
	uint16_t max = array[0];
	uint32_t i;
	
	for (i = 1; i < size; i++)
	{
		if (array[i] > max)
			max = data1[i];
	}
	
	return max;
}

uint16_t Get_Array_Min(uint16_t * array, uint32_t size)
{
	uint16_t min = array[0];
	uint32_t i;
	
	for (i = 1; i < size; i++)
	{
		if (array[i] < min)
			min = data1[i];
	}
	
	return min;
}




//����ģ��Ļ�ȡ
//if(HeartTime > 20)
//{
//	get_BMP();
//	sprintf(buff1,"cmd=2&uid=%s&topic=Heart&msg=#%3d#\r\n",PRODUCTID,BPM);
//	USART3_SendArray((u8*)buff1,strlen(buff1));
//	HeartTime = 0;
//}
