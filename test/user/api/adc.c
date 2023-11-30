#include "adc.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "delay.h"
//ADC�ĳ�ʼ������
//��������	PA5	ADC1_IN5
//����Ũ�ȴ�����MQ2	PC1	ADC1_IN11
u16 adc_data[2] = {0};
u8 adc_buff[100] = {0};
void ADC_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	//����ADC��ʱ��Ϊ12MHZ(������14MHZ)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//��ʱ��	GPIOA	GPIOC	ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC,ENABLE);
	
	//����GPIO�� PA5
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;//ģ������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_1;//ѡ������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//GPIO�ڵ��������
	//��ʼ��GPIO�� GPIOA
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	//����GPIO��	PC1
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//ѡ������
	//��ʼ��GPIO��	GPIOC
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
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

//��ȡ����ֵ
u16 Get_LightValue(void)
{
	//����ת��ͨ����ת��˳��ת��ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
	//��ʼת��
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	//�ȴ�ת�����
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==RESET);
	//��ȡ���һ��ADCת����ɺ��ֵ
	adc_data[0] = ADC_GetConversionValue(ADC1);
	printf("����:%d\r\n",adc_data[0]);
	sprintf((char*)adc_buff,"����:%d",adc_data[0]); // ƴ�ӹ��ռ��ֵ
	LCD_ShowStringAndHz(15,80,150,16,16,adc_buff,0); // �������(10,20)  �����С(150,16) �����С16
	return adc_data[0];
}

//��ȡ����Ũ��
u16 Get_SmokeValue(void)
{
	//����ת��ͨ����ת��˳��ת��ʱ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5);
	//��ʼת��
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	//�ȴ�ת�����
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	//��ȡ���һ��ADCת����ɺ��ֵ
	adc_data[1] = ADC_GetConversionValue(ADC1);
	printf("����:%d\r\n",adc_data[1]);
	sprintf((char*)adc_buff,"����:%d",adc_data[1]); // ƴ��������ֵ
	LCD_ShowStringAndHz(15,60,150,16,16,adc_buff,0); // �������(10,20)  �����С(150,16) �����С16
	return adc_data[1];
}








