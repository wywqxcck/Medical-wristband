/********************************Copyright (c)**********************************\
**
**                   (c) Copyright 2019, temp_duan, China, DY.
**                           All Rights Reserved
**
**                           By(�������ɿƼ����޹�˾)
**                             http://www.kpiot.top
**
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: usart3.c
** ������Ա: temp_duan
** ��������: 2019-07-09
** �ĵ�����:
**
**----------------------------------�汾��Ϣ------------------------------------
** �汾����: V0.1
** �汾˵��: ��ʼ�汾
**
**------------------------------------------------------------------------------
\********************************End of Head************************************/

/********************************End of File************************************/
#include "usart3.h"
/*******************************************************************************
** ��������: USART3_IRQHandler
** ��������:
** ����˵��: None
** ����˵��: None
** ������Ա: temp_duan
** ��������: 2019-07-09
**------------------------------------------------------------------------------
** �޸���Ա:
** �޸�����:
** �޸�����:
**------------------------------------------------------------------------------
********************************************************************************/\
extern double latitude,longitude;
extern u32 dis_hr_upload,dis_spo2_upload;
extern uint8_t hr_limit;

void Usart3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void Usart3_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Usart3_SendByte(Array[i]);
	}
}

void Usart3_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Usart3_SendByte(String[i]);
	}
}
void bluetooth_deal(uint8_t temp)
{
	switch(temp)
	{
		case 1:{char arr[30];sprintf(arr,"HR:%d SP:%d",dis_hr_upload,dis_spo2_upload);Usart3_SendString(arr);}break;
		case 2:{char arr[30];sprintf(arr,"latitude:%.3lf longitude:%.3lf",latitude,longitude);Usart3_SendString(arr);}break;
		case 3:hr_limit++;{char arr[30];sprintf(arr,"HR_Limit:%d",hr_limit);Usart3_SendString(arr);}break;
		case 4:hr_limit--;{char arr[30];sprintf(arr,"HR_Limit:%d",hr_limit);Usart3_SendString(arr);}break;
//		case 5:{char arr[30];sprintf(arr,"temp_limit:%d",temp_limit);Usart3_SendString(arr);}break;
//		case 6:{char arr[30];sprintf(arr,"set_limit:%d",set_limit);Usart3_SendString(arr);}break;
//		case 7:{char arr[30];sprintf(arr,"set_limit:%d",set_limit);Usart3_SendString(arr);}break;
	}
}

void USART3_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{
		res = USART_ReceiveData(USART3);
		bluetooth_deal(res);
	}
}

/*******************************************************************************
** ��������: usart3_init
** ��������:
** ����˵��: bound: [����/��]
** ����˵��: None
** ������Ա: temp_duan
** ��������: 2019-07-09
**------------------------------------------------------------------------------
** �޸���Ա:
** �޸�����:
** �޸�����:
**------------------------------------------------------------------------------
********************************************************************************/
void usart3_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	                       //GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);                         //����3ʱ��ʹ��

    //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                     //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                               //�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                         //��ʼ��PB10

    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                          //��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                         //��ʼ��PB11

    USART_InitStructure.USART_BaudRate = bound;                                    //������һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //�շ�ģʽ

    USART_Init(USART3, &USART_InitStructure);       //��ʼ������3


    USART_Cmd(USART3, ENABLE);                      //ʹ�ܴ���

    //ʹ�ܽ����ж�
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //�����ж�

    //�����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}

