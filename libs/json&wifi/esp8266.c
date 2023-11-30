#include "esp8266.h"
#include "stdio.h"
#include "delay.h"
#include "string.h"
#include "cJSON.h"
#include "stdlib.h"

USART3_DataStruct USART3_Data={0};
Weather_DataStruct Weather_Data[3]={0};
char location[20]="xinxiang";

void usart3_config(void)
{
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	//��IO��ģʽ
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	//��usart�Ĺ���ģʽ
	USART_InitTypeDef USART3_InitStruct;
	USART3_InitStruct.USART_BaudRate=115200;//������
	USART3_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//Ӳ������ʧ��
	USART3_InitStruct.USART_WordLength=USART_WordLength_8b;//���ݳ���
	USART3_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//����  ����
	USART3_InitStruct.USART_StopBits=USART_StopBits_1;//1λֹͣλ
	USART3_InitStruct.USART_Parity=USART_Parity_No;//����żУ��
	USART_Init(USART3,&USART3_InitStruct);
	USART_Cmd(USART3,ENABLE);//ʹ�ܴ���1
	
	//�����ж�����
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	//NVIC����
	NVIC_EnableIRQ(USART3_IRQn);
	NVIC_SetPriority(USART3_IRQn,1);
	
	WIFI_DISABLE;
	Delay_ms(50);
	WIFI_ENABLE;
}

void WIFI_Iinit(void)
{
	usart3_config();
	Delay_ms(2000);
	Connect_AP(MY_SSID,MY_PASSWORD);//����WiFi
}

void USART3_SendData(u8 data)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) == RESET)
	{}//�ȴ���һ֡���ݷ������
	USART_SendData(USART3,data);//��������
}

void USART3_SendStr(char * p)
{
	while(*p != '\0')
	{
		USART3_SendData(*p);
		p++;
	}
}

void USART3_SendArray(u8 Arr[] ,u16 DataLenth)
{
	u16 i=0;
	while(DataLenth --)
	{
		USART3_SendData(Arr[i++]);
	}
}

//�������շ�������ESP8266���ظ���Ƭ��������
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		u8 data = USART3->DR;
		USART1->DR = data;
		USART3_Data.RX_buff[USART3_Data.RX_count]=USART3->DR;
		USART3_Data.RX_count++;
		if(USART3_Data.RX_count > 1032)
			USART3_Data.RX_count=0;
	}
	if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)
	{
		u8 flag=0;
		flag=USART3->SR;
		flag=USART3->DR;//���ж�
		USART3_Data.USART_RevOverflag=1;//�������
	}
}

/************************************
�ַ�������
����1�����ĸ��ַ����в���
����2��Ҫ���ҵ�����
����3����ʱʱ��
����ֵ�����ҽ��
*************************************/
char * FindStr(char* dest, char* src, uint32_t timeout)
{
	while(timeout-- && (strstr(dest, src) == NULL))
	{
		Delay_ms(1);
	}
	return strstr(dest, src);
}

/**
 * ���ܣ�ESP8266����ָ���ȡӦ��
 * ������
 *       cmd -- ָ���ַ���
 *       ack -- Ӧ���ַ���
 *       timeout -- Ӧ�����ʱ��
 *			 check_cnt -- ѭ������ָ��Ĵ���������ģ��û��Ӧ�������෢���Σ�
 * ����ֵ��0 -- �ɹ�		>0 -- ʧ��
 */

uint8_t ESP8266_SendCmd_RecAck(char *cmd, char *ack, uint32_t timeout, uint8_t check_cnt)
{
	uint32_t len = strlen((char *)cmd);
	USART3_Data.RX_count = 0;
	memset(USART3_Data.RX_buff, 0, sizeof(USART3_Data.RX_buff));

	do{
		//��������
		if(cmd != NULL)
		{
			USART3_SendArray((uint8_t *)cmd, len);
			USART3_SendStr("\r\n");
		}
		//��ѯӦ���ַ���
		if(ack != NULL)
		{
			if(FindStr((char *)USART3_Data.RX_buff, ack, timeout) != NULL) //�ȴ����ڽ���Ӧ�𣬲�����
			{
				USART3_Data.RX_count = 0;
				return 0;//���յ���ȷӦ��
			}
		}
	}while(--check_cnt);
	return 1;//ʧ��
}

//�˳�͸��ģʽ
void Exit_Scotch(void)
{
		USART3_SendStr("+++");
		Delay_ms(500);
}

/************************************
STAģʽ ����2.4GHz���ȵ�
����1��SSID���ȵ���
����2��PASSWORD���ȵ�����
����ֵ��
*************************************/
u8 Connect_AP(char *SSID,char *PASSWORD)
{
	char sendarr[256]={0};
	Exit_Scotch();//�˳���͸ģʽ
	if(ESP8266_SendCmd_RecAck("AT","OK",500,2))
	{
		printf("AT ����ʧ��\r\n");
		return 1;
	}
	else
		printf("AT ���Գɹ�\r\n");
	if(ESP8266_SendCmd_RecAck("AT+CWMODE=1","OK",500,2))
	{
		printf("AT+CWMODE ����ʧ��\r\n");
		return 2;
	}
	else
		printf("AT+CWMODE ���óɹ�\r\n");
	sprintf(sendarr,"AT+CWJAP=\"%s\",\"%s\"",SSID,PASSWORD);
	if(ESP8266_SendCmd_RecAck(sendarr,"OK",10000,2))
	{
		printf("AT+CWJAP ����ʧ��\r\n");
		return 3;
	}
	else
	{
		printf("AT+CWJAP ���óɹ�\r\n");
		printf("�����ȵ�ɹ�\r\n");
	}
	return 0;
}

/************************************
���ӷ�����
����1��IP��������������IP��ַ
����2��PORT���������˿ں�
����ֵ��
*************************************/
u8 Connect_Server(char *IP,u16 PORT)
{
	char sendarr[256]={0};
	if(ESP8266_SendCmd_RecAck("AT+CWJAP?","OK",5000,2) == 1)
	{
		printf("δ��⵽�����ȵ�\r\n������...\r\n");
		if(Connect_AP(MY_SSID,MY_PASSWORD))
		{
			printf("δ��⵽�ȵ㣬�˳�\r\n");
			return 1;
		}
	}
	sprintf(sendarr,"AT+CIPSTART=\"TCP\",\"%s\",%d",IP,PORT);
	if(ESP8266_SendCmd_RecAck(sendarr,"OK",2000,2) == 1)
	{
		printf("���ӷ�����ʧ�ܣ��˳�\r\n");
		return 2;
	}
	if(ESP8266_SendCmd_RecAck("AT+CIPMODE=1","OK",500,2) == 1)
	{
		printf("����͸��ģʽʧ�ܣ��˳�\r\n");
		return 3;
	}
	if(ESP8266_SendCmd_RecAck("AT+CIPSEND",">",500,2) == 1)
	{
		printf("����͸��ģʽʧ�ܣ��˳�\r\n");
		return 4;
	}
	
	char sendbuff[512]={0};
	sprintf(sendbuff,"GET https://api.seniverse.com/v3/weather/daily.json?key=%s&location=%s&language=en&unit=c&start=0&days=5\r\n",MY_KEY,location);
	memset(&USART3_Data,0,sizeof(USART3_Data));
	USART3_SendStr(sendbuff);
	while(USART3_Data.USART_RevOverflag != 1)
	{}
	printf("\r\n****%s*****\r\n",USART3_Data.RX_buff);

	cJSON *root,*my_result,*my_zero,*my_location,*my_name,*my_daily,*my_0,*my_1,*my_2,*my_data,*my_text_day,*my_code_day,*my_text_night,*my_code_night,*my_high,*my_low,*my_humidity;
	
	root=cJSON_Parse((char *)USART3_Data.RX_buff);
	if(root == NULL)
	{
		printf("root failed");
		return 5;
	}
	
	my_result=cJSON_GetObjectItem(root,"results");//�ҳ�Ա
	if(my_result == NULL)
	{
		printf("my_result failed");
		cJSON_Delete(root);
		return 6;
	}	
	
	my_zero=cJSON_GetArrayItem(my_result,0);//������
	if(my_zero == NULL)
	{
		printf("my_zero failed");
		cJSON_Delete(root);
		return 7;
	}
	
	my_location=cJSON_GetObjectItem(my_zero,"location");
	if(my_location == NULL)
	{
		printf("my_location failed");
		cJSON_Delete(root);
		return 8;
	}
	
	my_name=cJSON_GetObjectItem(my_location,"name");
	if(my_name == NULL)
	{
		printf("my_name failed");
		cJSON_Delete(root);
		return 9;
	}
	
	my_daily=cJSON_GetObjectItem(my_zero,"daily");
	if(my_daily == NULL)
	{
		printf("my_daily failed");
		cJSON_Delete(root);
		return 10;
	}
	
	my_0=cJSON_GetArrayItem(my_daily,0);
	if(my_0 == NULL)
	{
		printf("my_0 failed");
		cJSON_Delete(root);
		return 11;
	}
	
	my_data=cJSON_GetObjectItem(my_0,"date");
	if(my_data == NULL)
	{
		printf("my_data failed");
		cJSON_Delete(root);
		return 12;
	}
	
	my_text_day=cJSON_GetObjectItem(my_0,"text_day");
	if(my_text_day == NULL)
	{
		printf("my_text_day failed");
		cJSON_Delete(root);
		return 13;
	}
	
	my_code_day=cJSON_GetObjectItem(my_0,"code_day");
	if(my_code_day == NULL)
	{
		printf("my_code_day failed");
		cJSON_Delete(root);
		return 14;
	}
	
	my_text_night=cJSON_GetObjectItem(my_0,"text_night");
	if(my_text_night == NULL)
	{
		printf("my_text_night failed");
		cJSON_Delete(root);
		return 15;
	}
	
	my_code_night=cJSON_GetObjectItem(my_0,"code_night");
	if(my_code_night == NULL)
	{
		printf("my_code_day failed");
		cJSON_Delete(root);
		return 16;
	}
	
	my_high=cJSON_GetObjectItem(my_0,"high");
	if(my_high == NULL)
	{
		printf("my_high failed");
		cJSON_Delete(root);
		return 17;
	}
	
	my_low=cJSON_GetObjectItem(my_0,"low");
	if(my_low == NULL)
	{
		printf("my_low failed");
		cJSON_Delete(root);
		return 18;
	}
	
	my_humidity=cJSON_GetObjectItem(my_0,"humidity");
	if(my_humidity == NULL)
	{
		printf("my_humidity failed");
		cJSON_Delete(root);
		return 19;
	}
	
	strcpy(Weather_Data[0].data,my_data->valuestring);
	strcpy(Weather_Data[0].name,my_name->valuestring);
	strcpy(Weather_Data[0].weatherday,my_text_day->valuestring);
	strcpy(Weather_Data[0].codeday,my_code_day->valuestring);
	strcpy(Weather_Data[0].weathernight,my_text_night->valuestring);
	strcpy(Weather_Data[0].codenight,my_code_night->valuestring);
	strcpy(Weather_Data[0].temhigh,my_high->valuestring);
	strcpy(Weather_Data[0].temlow,my_low->valuestring);
	strcpy(Weather_Data[0].humidity,my_humidity->valuestring);
	
	my_1=cJSON_GetArrayItem(my_daily,1);
	if(my_1 == NULL)
	{
		printf("my_1 failed");
		cJSON_Delete(root);
		return 20;
	}
	
	my_data=cJSON_GetObjectItem(my_1,"date");
	if(my_data == NULL)
	{
		printf("my_data failed");
		cJSON_Delete(root);
		return 21;
	}
	
	my_text_day=cJSON_GetObjectItem(my_1,"text_day");
	if(my_text_day == NULL)
	{
		printf("my_text_day failed");
		cJSON_Delete(root);
		return 22;
	}
	
	my_code_day=cJSON_GetObjectItem(my_1,"code_day");
	if(my_code_day == NULL)
	{
		printf("my_code_day failed");
		cJSON_Delete(root);
		return 23;
	}
	
	my_text_night=cJSON_GetObjectItem(my_1,"text_night");
	if(my_text_night == NULL)
	{
		printf("my_text_night failed");
		cJSON_Delete(root);
		return 24;
	}
	
	my_code_night=cJSON_GetObjectItem(my_1,"code_night");
	if(my_code_night == NULL)
	{
		printf("my_code_day failed");
		cJSON_Delete(root);
		return 25;
	}
	
	my_high=cJSON_GetObjectItem(my_1,"high");
	if(my_high == NULL)
	{
		printf("my_high failed");
		cJSON_Delete(root);
		return 26;
	}
	
	my_low=cJSON_GetObjectItem(my_1,"low");
	if(my_low == NULL)
	{
		printf("my_low failed");
		cJSON_Delete(root);
		return 27;
	}
	
	my_humidity=cJSON_GetObjectItem(my_1,"humidity");
	if(my_humidity == NULL)
	{
		printf("my_humidity failed");
		cJSON_Delete(root);
		return 28;
	}
	
	strcpy(Weather_Data[1].data,my_data->valuestring);
	strcpy(Weather_Data[1].name,my_name->valuestring);
	strcpy(Weather_Data[1].weatherday,my_text_day->valuestring);
	strcpy(Weather_Data[1].codeday,my_code_day->valuestring);
	strcpy(Weather_Data[1].weathernight,my_text_night->valuestring);
	strcpy(Weather_Data[1].codenight,my_code_night->valuestring);
	strcpy(Weather_Data[1].temhigh,my_high->valuestring);
	strcpy(Weather_Data[1].temlow,my_low->valuestring);
	strcpy(Weather_Data[1].humidity,my_humidity->valuestring);
	
	my_2=cJSON_GetArrayItem(my_daily,2);
	if(my_2 == NULL)
	{
		printf("my_2 failed");
		cJSON_Delete(root);
		return 29;
	}
	
	my_data=cJSON_GetObjectItem(my_2,"date");
	if(my_data == NULL)
	{
		printf("my_data failed");
		cJSON_Delete(root);
		return 30;
	}
	
	my_text_day=cJSON_GetObjectItem(my_2,"text_day");
	if(my_text_day == NULL)
	{
		printf("my_text_day failed");
		cJSON_Delete(root);
		return 31;
	}
	
	my_code_day=cJSON_GetObjectItem(my_2,"code_day");
	if(my_code_day == NULL)
	{
		printf("my_code_day failed");
		cJSON_Delete(root);
		return 32;
	}
	
	my_text_night=cJSON_GetObjectItem(my_2,"text_night");
	if(my_text_night == NULL)
	{
		printf("my_text_night failed");
		cJSON_Delete(root);
		return 33;
	}
	
	my_code_night=cJSON_GetObjectItem(my_2,"code_night");
	if(my_code_night == NULL)
	{
		printf("my_code_day failed");
		cJSON_Delete(root);
		return 34;
	}
	
	my_high=cJSON_GetObjectItem(my_2,"high");
	if(my_high == NULL)
	{
		printf("my_high failed");
		cJSON_Delete(root);
		return 35;
	}
	
	my_low=cJSON_GetObjectItem(my_2,"low");
	if(my_low == NULL)
	{
		printf("my_low failed");
		cJSON_Delete(root);
		return 36;
	}
	
	my_humidity=cJSON_GetObjectItem(my_2,"humidity");
	if(my_humidity == NULL)
	{
		printf("my_humidity failed");
		cJSON_Delete(root);
		return 37;
	}
	
	strcpy(Weather_Data[2].data,my_data->valuestring);
	strcpy(Weather_Data[2].name,my_name->valuestring);
	strcpy(Weather_Data[2].weatherday,my_text_day->valuestring);
	strcpy(Weather_Data[2].codeday,my_code_day->valuestring);
	strcpy(Weather_Data[2].weathernight,my_text_night->valuestring);
	strcpy(Weather_Data[2].codenight,my_code_night->valuestring);
	strcpy(Weather_Data[2].temhigh,my_high->valuestring);
	strcpy(Weather_Data[2].temlow,my_low->valuestring);
	strcpy(Weather_Data[2].humidity,my_humidity->valuestring);
	
	u8 n;
	for(n=0;n<3;n++)
	{
		printf("[data:%s]\r\n",Weather_Data[n].data);
		printf("[name:%s]\r\n",Weather_Data[n].name);
		printf("[weatherday:%s]\r\n",Weather_Data[n].weatherday);
		printf("[codeday:%s]\r\n",Weather_Data[n].codeday);
		printf("[weathernight:%s]\r\n",Weather_Data[n].weathernight);
		printf("[codenight:%s]\r\n",Weather_Data[n].codenight);
		printf("[high:%s]\r\n",Weather_Data[n].temhigh);
		printf("[lowt:%s]\r\n",Weather_Data[n].temlow);
		printf("[humidity:%s]\r\n",Weather_Data[n].humidity);
	}

	cJSON_Delete(root);//�ͷſ��ٵĿռ�
	return 0;
}


