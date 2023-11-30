#include "my1680.h"
#include "string.h"
#include "esp8266.h"
#include "stdlib.h"
#include "dht11.h"
#include "adc.h"


void MY1680_Init(void)
{
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	//配IO口模式
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF_PP;//复用推挽
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//配usart的工作模式
	USART_InitTypeDef USART4_InitStruct;
	USART4_InitStruct.USART_BaudRate=9600;//波特率
	USART4_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//硬件控制失能
	USART4_InitStruct.USART_WordLength=USART_WordLength_8b;//数据长度
	USART4_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//接受  发送
	USART4_InitStruct.USART_StopBits=USART_StopBits_1;//1位停止位
	USART4_InitStruct.USART_Parity=USART_Parity_No;//关奇偶校验
	USART_Init(UART4,&USART4_InitStruct);
	USART_Cmd(UART4,ENABLE);//使能串口1
}

//发送单个字节
void USART4_SendData(u8 data)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC) == RESET)
	{}//等待上一帧数据发送完成
	USART_SendData(UART4,data);//发送数据
}

//发送以‘\0’结尾的字符串
void USART4_SendStr(char * p)
{
	while(*p != '\0')
	{
		USART4_SendData(*p);
		p++;
	}
}

//发送定长数组
void USART4_SendArray(u8 Arr[] ,u16 DataLenth)
{
	u16 i=0;
	while(DataLenth --)
	{
		USART4_SendData(Arr[i++]);
	}
}

/************************************
播放MY1680芯片根目录中的歌曲
参数1：曲目名称高位
参数2：曲目名称低位
备注：曲目名称命名格式0001歌曲名.mp3
*************************************/
void PlayMP3_41(u8 H,u8 L)
{
	USART4_SendData(0x7E);
	USART4_SendData(0x05);
	USART4_SendData(0x41);
	USART4_SendData(H);
	USART4_SendData(L);
	u8 cheak = (0x05^0x41^H^L);
	USART4_SendData(cheak);
	USART4_SendData(0xEF);
}

/************************************
播放MY1680芯片目录中的歌曲
参数1：目录名称
参数2：曲目名称低位
备注：目录名称命名格式00-99；曲目名称命名格式001 歌曲名.mp3
*************************************/
void PlayMP3_42(u8 DIR,u8 L)
{
	USART4_SendData(0x7E);
	USART4_SendData(0x05);
	USART4_SendData(0x42);
	USART4_SendData(DIR);
	USART4_SendData(L);
	u8 cheak = (0x05^0x42^DIR^L);
	USART4_SendData(cheak);
	USART4_SendData(0xEF);
}

/************************************
播放数字
参数；要播放的数字(-255~255)
*************************************/
void PlayMP3_T(int t)
{
	u8 i,j,k;
	if(t<0)
	{
		PlayMP3_42(00,24);
		t = -t;
	}
	if(t<20)
	{
		PlayMP3_42(00,t);
		return;
	}
	k=t/100;//判断百位
	i=(t%100)/10;//判断十位
	j=t%10;//判断个位
	if(k != 0)
	{
			PlayMP3_42(00,k);
			PlayMP3_42(00,20);
	}
	if(i != 0)
	{
		PlayMP3_42(00,i);
		PlayMP3_42(00,10);
	}
	if(j != 0)
	{	
		if(i == 0)
		{
			PlayMP3_42(00,00);
		}
		PlayMP3_42(00,j);
	}
}

//设置音量(0-30)
void PlayMP3_SetVol(u8 v)
{
	if(v>30)
		v=30;
	USART4_SendData(0x7E);
	USART4_SendData(0x04);
	USART4_SendData(0x31);
	USART4_SendData(v);
	u8 cheak = (0x05^0x42^v);
	USART4_SendData(cheak);
	USART4_SendData(0xEF);
}

//播报今天天气
/*void PlayMP3_Weather1(void)
{
	PlayMP3_42(06,00);
	PlayMP3_42(04,04);
	PlayMP3_42(02,03);
	//PlayMP3_42(02,atoi(Weather_Data[0].codeday));//atoi把字符X转换为数字X
	PlayMP3_42(02,02);
	//PlayMP3_42(02,atoi(Weather_Data[0].codenight));
	int i=0;
	PlayMP3_42(04,02);
	//i=atoi(Weather_Data[0].temhigh);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
	PlayMP3_42(04,03);
	//i=atoi(Weather_Data[0].temlow);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
}


//播报明天天气
void PlayMP3_Weather2(void)
{
	PlayMP3_42(06,00);
	PlayMP3_42(04,05);
	PlayMP3_42(02,03);
	PlayMP3_42(02,atoi(Weather_Data[1].codeday));
	PlayMP3_42(02,02);
	PlayMP3_42(02,atoi(Weather_Data[1].codenight));
	int i=0;
	PlayMP3_42(04,02);
	i=atoi(Weather_Data[1].temhigh);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
	PlayMP3_42(04,03);
	i=atoi(Weather_Data[1].temlow);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
}

//播报后天天气
void PlayMP3_Weather3(void)
{
	PlayMP3_42(06,00);
	PlayMP3_42(04,06);
	PlayMP3_42(02,03);
	PlayMP3_42(02,atoi(Weather_Data[2].codeday));
	PlayMP3_42(02,02);
	PlayMP3_42(02,atoi(Weather_Data[2].codenight));
	int i=0;
	PlayMP3_42(04,02);
	i=atoi(Weather_Data[2].temhigh);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
	PlayMP3_42(04,03);
	i=atoi(Weather_Data[2].temlow);
	PlayMP3_T(i);
	PlayMP3_42(01,01);
}*/
//播报当前温度
void PlayMP3_Tem(void)
{
	PlayMP3_42(01,05); // 当前室内温度
	PlayMP3_T(Tem); // 温度值
	PlayMP3_42(01,01); // 摄氏度
}
//播报当前湿度
void PlayMP3_Hum(void)
{
	PlayMP3_42(01,06); // 当前室内湿度
	PlayMP3_42(01,02); // 百分之
	PlayMP3_T(Hum); // 湿度值
}
//播报当前光照强度
void PlayMP3_Light(void)
{ 
	PlayMP3_42(01,07); // 当前室内光照强度
	PlayMP3_T(adc_data[0]/1000); // 光照强度千位数值
	PlayMP3_42(00,21); // 千
	
	if((adc_data[0]/100)%10==0)
	{
		PlayMP3_42(00,000);
	}
	
	PlayMP3_T(adc_data[0]%1000);
	PlayMP3_42(01,03); // LUX
}
//播报当前烟雾浓度
void PlayMP3_Smoke(void)
{
	PlayMP3_42(01,8); // 当前室内烟雾浓度
	PlayMP3_T(adc_data[1]); // 烟雾浓度值
	PlayMP3_42(01,04); // PPM
}
