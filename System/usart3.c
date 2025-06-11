/********************************Copyright (c)**********************************\
**
**                   (c) Copyright 2019, temp_duan, China, DY.
**                           All Rights Reserved
**
**                           By(德阳科派科技有限公司)
**                             http://www.kpiot.top
**
**----------------------------------文件信息------------------------------------
** 文件名称: usart3.c
** 创建人员: temp_duan
** 创建日期: 2019-07-09
** 文档描述:
**
**----------------------------------版本信息------------------------------------
** 版本代号: V0.1
** 版本说明: 初始版本
**
**------------------------------------------------------------------------------
\********************************End of Head************************************/

/********************************End of File************************************/
#include "usart3.h"
/*******************************************************************************
** 函数名称: USART3_IRQHandler
** 功能描述:
** 参数说明: None
** 返回说明: None
** 创建人员: temp_duan
** 创建日期: 2019-07-09
**------------------------------------------------------------------------------
** 修改人员:
** 修改日期:
** 修改描述:
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
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{
		res = USART_ReceiveData(USART3);
		bluetooth_deal(res);
	}
}

/*******************************************************************************
** 函数名称: usart3_init
** 功能描述:
** 参数说明: bound: [输入/出]
** 返回说明: None
** 创建人员: temp_duan
** 创建日期: 2019-07-09
**------------------------------------------------------------------------------
** 修改人员:
** 修改日期:
** 修改描述:
**------------------------------------------------------------------------------
********************************************************************************/
void usart3_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	                       //GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);                         //串口3时钟使能

    //USART3_TX   PB10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                                     //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	                               //复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                         //初始化PB10

    //USART3_RX	  PB11
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;                          //浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                         //初始化PB11

    USART_InitStructure.USART_BaudRate = bound;                                    //波特率一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                            //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	               //收发模式

    USART_Init(USART3, &USART_InitStructure);       //初始化串口3


    USART_Cmd(USART3, ENABLE);                      //使能串口

    //使能接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //开启中断

    //设置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

}

