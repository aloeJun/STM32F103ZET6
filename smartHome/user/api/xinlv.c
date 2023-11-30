#include "xinlv.h"
#include "stdio.h"
#include "bsp_lcd.h"

char BMP_show[50] = {0};

uint16_t readData, preReadData;		   		// 读取到的 AD 值
uint16_t timeCount = 0;         // 采样周期计数变量
uint16_t firstTimeCount = 0;    // 第一个心跳时间
uint16_t secondTimeCount = 0;   // 第二个心跳时间

typedef enum {FALSE, TRUE} BOOL;

/* 发送给上位机的三个量 IBI: 相邻两个心跳的时间，BPM: 心率值， SIG: 脉象图的数值化表示 */
uint16_t IBI, BPM, SIG;   		

uint16_t data1[50] = {0}; // 采样数据缓存
int a = 0; 				// 缓存数组下标
uint16_t max, min, mid;			// 最大、最小及中间值
uint16_t filter;				// 滤波值

BOOL PULSE = FALSE;				// 有效脉搏
BOOL PRE_PULSE = FALSE;         // 先前有效脉搏

uint8_t pulseCount = 0;			// 脉搏计数


//PA1 ADC1_IN1 心率模块
void Heart_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	ADC_InitTypeDef ADC_InitStruct = {0};
	//配置ADC的时钟为12MHZ(不超过14MHZ)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//开时钟	GPIOA		ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
	
	//配置GPIO口 PA1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;//选择引脚
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//GPIO口的输出速率
	//初始化GPIO口 GPIOA
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	
	
	//配置ADC
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;//不使用连续转换
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//不使用硬件流控
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;//独立工作模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;//待转换通道数量
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;//不使用扫描模式
	//初始化ADC
	ADC_Init(ADC1, &ADC_InitStruct);
	//使能ADC
	ADC_Cmd(ADC1,ENABLE);
	
	//重置校准寄存器
	ADC_ResetCalibration(ADC1);
	//等待校准寄存器重置完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	//校准ADC
	ADC_StartCalibration(ADC1);
	//等待ADC校准完成
	while(ADC_GetCalibrationStatus(ADC1));
}




u16 Get_LightVale(void)
{
	u8 i,j;
	u16 buff[10] = {0};
	u16 temp;
	
	//配置通道	规则组	转换顺序	转换时间
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);//转换时间越长，数据就越准确
	
	//连续转换十次
	for(i = 0; i < 10; i ++)
	{
		//软件控制开始转换
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		//等待转换完成
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == 0);
		//获取转换数据
		buff[i] = ADC_GetConversionValue(ADC1);
	}
	
	//把读取值的数据按从小到大的排列，冒泡排序
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
	
	//去掉一个最大值，去掉一个最小值求平均值
	temp = 0;
	for(i = 1; i < 9; i++)
	{
		temp += buff[i];
	}
	
	return temp/8;
}

void get_BMP(void)
{
		preReadData = readData;	        // 保存前一次值
		readData = Get_LightVale();		// 读取AD转换值
		
		if ((readData - preReadData) < filter)    // 滤除突变噪声信号干扰
			data1[a++] = readData;	// 填充缓存数组

		if (a >= 50)
		{	
			a = 0;	// 数组填满，从头再填
		
			// 通过缓存数组获取脉冲信号的波峰、波谷值，并计算中间值作为判定参考阈值
			max = Get_Array_Max(data1, 50);
			min = Get_Array_Min(data1, 50);
			mid = (max + min)/2;
			filter = (max - min) / 2;
		}
		
		PRE_PULSE = PULSE;	// 保存当前脉冲状态
		PULSE = (readData > mid) ? TRUE : FALSE;  // 采样值大于中间值为有效脉冲
		
		if (PRE_PULSE == FALSE && PULSE == TRUE)  // 寻找到“信号上升到振幅中间位置”的特征点，检测到一次有效脉搏
		{	
			pulseCount++;
			pulseCount %= 2;	 
			
			if(pulseCount == 1) // 两次脉搏的第一次
			{                         	
				firstTimeCount = timeCount;   // 记录第一次脉搏时间
			}
			if(pulseCount == 0)  // 两次脉搏的第二次
			{                             			
				secondTimeCount = timeCount;  // 记录第二次脉搏时间
				timeCount = 0;	

				if ( (secondTimeCount > firstTimeCount))
				{
					IBI = (secondTimeCount - firstTimeCount) * 20;	// 计算相邻两次脉搏的时间，得到 IBI
					BPM = 60000 / IBI;  // 通过 IBI 得到心率值 BPM
					
					if (BPM > 200)    //限制BPM最高显示值
						BPM = 200;	 				
					if (BPM < 30)    //限制BPM最低显示值
						BPM=30;
				}
			
			}
				printf("BMP:%3d\r\n",BPM);//心率值数据打印
				sprintf(BMP_show,"BMP:%3d",BPM);
				LCD_ShowStringAndHz(15,120,80,16,16,(u8*)BMP_show,0);
				
		}

		timeCount++;  // 时间计数累加
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




//心率模块的获取
//if(HeartTime > 20)
//{
//	get_BMP();
//	sprintf(buff1,"cmd=2&uid=%s&topic=Heart&msg=#%3d#\r\n",PRODUCTID,BPM);
//	USART3_SendArray((u8*)buff1,strlen(buff1));
//	HeartTime = 0;
//}
