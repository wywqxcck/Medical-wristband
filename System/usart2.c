/********************************Copyright (c)**********************************\
**
**                   (c) Copyright 2019, temp_duan, China, DY.
**                           All Rights Reserved
**
**                           By(�������ɿƼ����޹�˾)
**                             http://www.kpiot.top
**
**----------------------------------�ļ���Ϣ------------------------------------
** �ļ�����: usart2.c
** ������Ա: temp_duan
** ��������: 2019-08-01
** �ĵ�����: 
**
**----------------------------------�汾��Ϣ------------------------------------
** �汾����: V0.1
** �汾˵��: ��ʼ�汾
**
**------------------------------------------------------------------------------
\********************************End of Head************************************/

#include "usart2.h"
/*******************************************************************************
** ��������: usart2_init
** ��������: 
** ����˵��: bound: [����/��] 
** ����˵��: None
** ������Ա: temp_duan
** ��������: 2019-08-01
**------------------------------------------------------------------------------
** �޸���Ա:
** �޸�����:
** �޸�����:
**------------------------------------------------------------------------------
********************************************************************************/
void usart2_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	                       //GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);                         //����2ʱ��ʹ��

    //USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                                     //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                               //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //��ʼ��PA2

    //USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                          //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                         //��ʼ��PA3

    USART_InitStructure.USART_BaudRate = bound;                                    //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure);       //��ʼ������2


    USART_Cmd(USART2, ENABLE);                      //ʹ�ܴ���

    //ʹ�ܽ����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //�����ж�

    //�����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //��ռ���ȼ�0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}

/*******************************************************************************
** ��������: USART2_IRQHandler
** ��������: 
** ����˵��: None
** ����˵��: None
** ������Ա: temp_duan
** ��������: 2019-08-01
**------------------------------------------------------------------------------
** �޸���Ա:
** �޸�����:
** �޸�����:
**------------------------------------------------------------------------------
********************************************************************************/
extern short int point1;
extern const short int USART2_MAX_RECV_LEN;
extern char USAR2_RX_BUF[200];
extern const short int GPS_Buffer_Length;
short int point1;
extern struct Data Save_Data;

void USART2_IRQHandler(void){
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
			res = USART_ReceiveData(USART2);
			if(res == '$'){
				point1 = 0;
			}
			USAR2_RX_BUF[point1++] = res;
			if(USAR2_RX_BUF[0] == '$' && USAR2_RX_BUF[4] == 'M' && USAR2_RX_BUF[5] == 'C'){
				if(res == '\n'){
					memcpy(Save_Data.GPS_Buffer, USAR2_RX_BUF, point1);
					Save_Data.isGetData = true;
					point1 = 0;
					memset(USAR2_RX_BUF, 0, USART2_MAX_RECV_LEN);//????��y����
					
					parseGpsBuffer();//?a??��y?Y
					printfGpsBuffer();//�䨰��?��y?Y
				}
			}
			if(point1 >= USART2_MAX_RECV_LEN)
			{
				point1 = USART2_MAX_RECV_LEN;
			}
	}
}

//int fputc(int ch,FILE *p)  //����Ĭ�ϵģ���ʹ��printf����ʱ�Զ�����
//{
//	USART_SendData(USART2,(u8)ch);	
//	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
//	return ch;
//}
