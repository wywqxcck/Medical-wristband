#include "max30102.h"
#include "Delay.h"
#include "Delay.h"
#include "sys.h"
#include "stdio.h"	 
#include "OLED.h"
#include "string.h" 	
#include "stm32f10x.h"                  // Device header


#define MAX_BRIGHTNESS 255
#define INTERRUPT_REG 0X00

uint32_t aun_ir_buffer[500]; 	 //IR LED   ��������ݣ����ڼ���Ѫ��
int32_t n_ir_buffer_length;    //���ݳ���
uint32_t aun_red_buffer[500];  //Red LED	������ݣ����ڼ������������Լ���������
int32_t n_sp02; //SPO2ֵ
int8_t ch_spo2_valid;   //������ʾSP02�����Ƿ���Ч��ָʾ��
int32_t n_heart_rate;   //����ֵ
int8_t  ch_hr_valid;    //������ʾ���ʼ����Ƿ���Ч��ָʾ��
 
uint8_t Temp;
 
uint32_t un_min, un_max, un_prev_data;  
int i;
int32_t n_brightness;
float f_temp;
//u8 temp_num=0;
u8 temp[6];
u8 str[100];
int32_t dis_hr=0,dis_spo2=0;
int32_t dis_hr_upload=0,dis_spo2_upload=0;

u8 max30102_Bus_Write(u8 Register_Address, u8 Word_Data)
{
 
	/* ���ô���EEPROM�漴��ȡָ�����У�������ȡ�����ֽ� */
 
	/* ��1��������I2C���������ź� */
	MAX30102_IIC_Start();
 
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */
 
	/* ��3��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��4���������ֽڵ�ַ */
	MAX30102_IIC_Send_Byte(Register_Address);
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	
	/* ��5������ʼд������ */
	MAX30102_IIC_Send_Byte(Word_Data);
 
	/* ��6��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
	return 1;	/* ִ�гɹ� */
 
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
	return 0;
}
 
 
 
u8 max30102_Bus_Read(u8 Register_Address)
{
	u8  data;
 
 
	/* ��1��������I2C���������ź� */
	MAX30102_IIC_Start();
 
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */
 
	/* ��3��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��4���������ֽڵ�ַ�� */
	MAX30102_IIC_Send_Byte((uint8_t)Register_Address);
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	
 
	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	MAX30102_IIC_Start();
 
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */
 
	/* ��8��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��9������ȡ���� */
	{
		data = MAX30102_IIC_Read_Byte(0);	/* ��1���ֽ� */
 
		MAX30102_IIC_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
	}
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
	return data;	/* ִ�гɹ� ����dataֵ */
 
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
	return 0;
}
 
 
void max30102_FIFO_ReadWords(u8 Register_Address,u16 Word_Data[][2],u8 count)
{
	u8 i=0;
	u8 no = count;
	u8 data1, data2;
	/* ��1��������I2C���������ź� */
	MAX30102_IIC_Start();
 
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */
 
	/* ��3��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��4���������ֽڵ�ַ�� */
	MAX30102_IIC_Send_Byte((uint8_t)Register_Address);
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	
 
	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	MAX30102_IIC_Start();
 
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */
 
	/* ��8��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��9������ȡ���� */
	while (no)
	{
		data1 = MAX30102_IIC_Read_Byte(0);	
		MAX30102_IIC_Ack();
		data2 = MAX30102_IIC_Read_Byte(0);
		MAX30102_IIC_Ack();
		Word_Data[i][0] = (((u16)data1 << 8) | data2);  //
 
		
		data1 = MAX30102_IIC_Read_Byte(0);	
		MAX30102_IIC_Ack();
		data2 = MAX30102_IIC_Read_Byte(0);
		if(1==no)
			MAX30102_IIC_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
		else
			MAX30102_IIC_Ack();
		Word_Data[i][1] = (((u16)data1 << 8) | data2); 
 
		no--;	
		i++;
	}
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
 
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
}
 
void max30102_FIFO_ReadBytes(u8 Register_Address,u8* Data)
{	
	max30102_Bus_Read(REG_INTR_STATUS_1);
	max30102_Bus_Read(REG_INTR_STATUS_2);
	
	/* ��1��������I2C���������ź� */
	MAX30102_IIC_Start();
 
	/* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */
 
	/* ��3��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��4���������ֽڵ�ַ�� */
	MAX30102_IIC_Send_Byte((uint8_t)Register_Address);
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
	
 
	/* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
	MAX30102_IIC_Start();
 
	/* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
	MAX30102_IIC_Send_Byte(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */
 
	/* ��8��������ACK */
	if (MAX30102_IIC_Wait_Ack() != 0)
	{
		goto cmd_fail;	/* EEPROM������Ӧ�� */
	}
 
	/* ��9������ȡ���� */
	Data[0] = MAX30102_IIC_Read_Byte(1);	
	Data[1] = MAX30102_IIC_Read_Byte(1);	
	Data[2] = MAX30102_IIC_Read_Byte(1);	
	Data[3] = MAX30102_IIC_Read_Byte(1);
	Data[4] = MAX30102_IIC_Read_Byte(1);	
	Data[5] = MAX30102_IIC_Read_Byte(0);
	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
 
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
	/* ����I2C����ֹͣ�ź� */
	MAX30102_IIC_Stop();
 
//	u8 i;
//	u8 fifo_wr_ptr;
//	u8 firo_rd_ptr;
//	u8 number_tp_read;
//	//Get the FIFO_WR_PTR
//	fifo_wr_ptr = max30102_Bus_Read(REG_FIFO_WR_PTR);
//	//Get the FIFO_RD_PTR
//	firo_rd_ptr = max30102_Bus_Read(REG_FIFO_RD_PTR);
//	
//	number_tp_read = fifo_wr_ptr - firo_rd_ptr;
//	
//	//for(i=0;i<number_tp_read;i++){
//	if(number_tp_read>0){
//		MAX30102_IIC_ReadBytes(max30102_WR_address,REG_FIFO_DATA,Data,6);
//	}
	
	//max30102_Bus_Write(REG_FIFO_RD_PTR,fifo_wr_ptr);
}
 
void MAX30102_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	MAX30102_IIC_Init();
	
	MAX30102_Reset();
	
//	max30102_Bus_Write(REG_MODE_CONFIG, 0x0b);  //mode configuration : temp_en[3]      MODE[2:0]=010 HR only enabled    011 SP02 enabled
//	max30102_Bus_Write(REG_INTR_STATUS_2, 0xF0); //open all of interrupt
//	max30102_Bus_Write(REG_INTR_STATUS_1, 0x00); //all interrupt clear
//	max30102_Bus_Write(REG_INTR_ENABLE_2, 0x02); //DIE_TEMP_RDY_EN
//	max30102_Bus_Write(REG_TEMP_CONFIG, 0x01); //SET   TEMP_EN
 
//	max30102_Bus_Write(REG_SPO2_CONFIG, 0x47); //SPO2_SR[4:2]=001  100 per second    LED_PW[1:0]=11  16BITS
 
//	max30102_Bus_Write(REG_LED1_PA, 0x47); 
//	max30102_Bus_Write(REG_LED2_PA, 0x47); 
	
	
	
	max30102_Bus_Write(REG_INTR_ENABLE_1,0xc0);	// INTR setting
	max30102_Bus_Write(REG_INTR_ENABLE_2,0x00);
	max30102_Bus_Write(REG_FIFO_WR_PTR,0x00);  	//FIFO_WR_PTR[4:0]
	max30102_Bus_Write(REG_OVF_COUNTER,0x00);  	//OVF_COUNTER[4:0]
	max30102_Bus_Write(REG_FIFO_RD_PTR,0x00);  	//FIFO_RD_PTR[4:0]
	max30102_Bus_Write(REG_FIFO_CONFIG,0x0f);  	//sample avg = 1, fifo rollover=false, fifo almost full = 17
	max30102_Bus_Write(REG_MODE_CONFIG,0x03);  	//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	max30102_Bus_Write(REG_SPO2_CONFIG,0x27);  	// SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)  
	max30102_Bus_Write(REG_LED1_PA,0x24);   	//Choose value for ~ 7mA for LED1
	max30102_Bus_Write(REG_LED2_PA,0x24);   	// Choose value for ~ 7mA for LED2
	max30102_Bus_Write(REG_PILOT_PA,0x7f);   	// Choose value for ~ 25mA for Pilot LED
 
	printf("\r\n MAX30102  init  \r\n");
}
 
void MAX30102_Reset(void)
{
	max30102_Bus_Write(REG_MODE_CONFIG,0x40);
	max30102_Bus_Write(REG_MODE_CONFIG,0x40);
}
 
 
 
 
 
 
void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
{
//  char ach_i2c_data[2];
//  ach_i2c_data[0]=uch_addr;
//  ach_i2c_data[1]=uch_data;
//	
//  MAX30102_IIC_WriteBytes(I2C_WRITE_ADDR, ach_i2c_data, 2);
	MAX30102_IIC_Write_One_Byte(I2C_WRITE_ADDR,uch_addr,uch_data);
}
 
void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
{
//  char ch_i2c_data;
//  ch_i2c_data=uch_addr;
//  MAX30102_IIC_WriteBytes(I2C_WRITE_ADDR, &ch_i2c_data, 1);
//	
//  i2c.read(I2C_READ_ADDR, &ch_i2c_data, 1);
//  
//   *puch_data=(uint8_t) ch_i2c_data;
	MAX30102_IIC_Read_One_Byte(I2C_WRITE_ADDR,uch_addr,puch_data);
}
 
void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)
{
	uint32_t un_temp;
	unsigned char uch_temp;
	char ach_i2c_data[6];
	*pun_red_led=0;
	*pun_ir_led=0;
 
  
  //read and clear status register
  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
  maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);
  
  MAX30102_IIC_ReadBytes(I2C_WRITE_ADDR,REG_FIFO_DATA,(u8 *)ach_i2c_data,6);
  
  un_temp=(unsigned char) ach_i2c_data[0];
  un_temp<<=16;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[1];
  un_temp<<=8;
  *pun_red_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[2];
  *pun_red_led+=un_temp;
  
  un_temp=(unsigned char) ach_i2c_data[3];
  un_temp<<=16;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[4];
  un_temp<<=8;
  *pun_ir_led+=un_temp;
  un_temp=(unsigned char) ach_i2c_data[5];
  *pun_ir_led+=un_temp;
  *pun_red_led&=0x03FFFF;  //Mask MSB [23:18]
  *pun_ir_led&=0x03FFFF;  //Mask MSB [23:18]
}
 
 
 
 
//MAX30102�������ģʽ����
void MAX30102_IIC_SDA_OUT(void)//SDA�����������
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=MAX30102_IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;//SDA�������
	GPIO_Init(MAX30102_IIC_PORT,&GPIO_InitStructure); 						
 
}
 
void MAX30102_IIC_SDA_IN(void)//SDA���뷽������
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	
	GPIO_InitStructure.GPIO_Pin=MAX30102_IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;//SCL��������
	GPIO_Init(MAX30102_IIC_PORT,&GPIO_InitStructure);
	
}
 
//��ʼ��IIC
void MAX30102_IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = MAX30102_IIC_SCL_PIN|MAX30102_IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MAX30102_IIC_PORT, &GPIO_InitStructure);
 
	MAX30102_IIC_SCL=1;
	MAX30102_IIC_SDA=1;
 
}
//����IIC��ʼ�ź�
void MAX30102_IIC_Start(void)
{
	MAX30102_IIC_SDA_OUT();     //sda�����
	MAX30102_IIC_SDA=1;	  	  
	MAX30102_IIC_SCL=1;
	Delay_us(4);
 	MAX30102_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	Delay_us(4);
	MAX30102_IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void MAX30102_IIC_Stop(void)
{
	MAX30102_IIC_SDA_OUT();//sda�����
	MAX30102_IIC_SCL=0;
	MAX30102_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	Delay_us(4);
	MAX30102_IIC_SCL=1; 
	MAX30102_IIC_SDA=1;//����I2C���߽����ź�
	Delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 MAX30102_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	MAX30102_IIC_SDA_IN();      //SDA����Ϊ����  
	MAX30102_IIC_SDA=1;Delay_us(1);	   
	MAX30102_IIC_SCL=1;Delay_us(1);	 
	while(MAX30102_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			MAX30102_IIC_Stop();
			return 1;
		}
	}
	MAX30102_IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void MAX30102_IIC_Ack(void)
{
	MAX30102_IIC_SCL=0;
	MAX30102_IIC_SDA_OUT();
	MAX30102_IIC_SDA=0;
	Delay_us(2);
	MAX30102_IIC_SCL=1;
	Delay_us(2);
	MAX30102_IIC_SCL=0;
}
//������ACKӦ��		    
void MAX30102_IIC_NAck(void)
{
	MAX30102_IIC_SCL=0;
	MAX30102_IIC_SDA_OUT();
	MAX30102_IIC_SDA=1;
	Delay_us(2);
	MAX30102_IIC_SCL=1;
	Delay_us(2);
	MAX30102_IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void MAX30102_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	MAX30102_IIC_SDA_OUT(); 	    
    MAX30102_IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        MAX30102_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		Delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		MAX30102_IIC_SCL=1;
		Delay_us(2); 
		MAX30102_IIC_SCL=0;	
		Delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 MAX30102_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	MAX30102_IIC_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        MAX30102_IIC_SCL=0; 
        Delay_us(2);
		MAX30102_IIC_SCL=1;
        receive<<=1;
        if(MAX30102_READ_SDA)receive++;   
		Delay_us(1); 
    }					 
    if (!ack)
        MAX30102_IIC_NAck();//����nACK
    else
        MAX30102_IIC_Ack(); //����ACK   
    return receive;
}
 
 
void MAX30102_IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    MAX30102_IIC_Start();  
 
	MAX30102_IIC_Send_Byte(WriteAddr);	    //����д����
	MAX30102_IIC_Wait_Ack();
	
	for(i=0;i<dataLength;i++)
	{
		MAX30102_IIC_Send_Byte(data[i]);
		MAX30102_IIC_Wait_Ack();
	}				    	   
    MAX30102_IIC_Stop();//����һ��ֹͣ���� 
	Delay_ms(10);	 
}
 
void MAX30102_IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength)
{		
	u8 i;	
    MAX30102_IIC_Start();  
 
	MAX30102_IIC_Send_Byte(deviceAddr);	    //����д����
	MAX30102_IIC_Wait_Ack();
	MAX30102_IIC_Send_Byte(writeAddr);
	MAX30102_IIC_Wait_Ack();
	MAX30102_IIC_Send_Byte(deviceAddr|0X01);//�������ģʽ			   
	MAX30102_IIC_Wait_Ack();
	
	for(i=0;i<dataLength-1;i++)
	{
		data[i] = MAX30102_IIC_Read_Byte(1);
	}		
	data[dataLength-1] = MAX30102_IIC_Read_Byte(0);	
    MAX30102_IIC_Stop();//����һ��ֹͣ���� 
	Delay_ms(10);	 
}
 
void MAX30102_IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data)
{				  	  	    																 
    MAX30102_IIC_Start();  
	
	MAX30102_IIC_Send_Byte(daddr);	   //����д����
	MAX30102_IIC_Wait_Ack();
	MAX30102_IIC_Send_Byte(addr);//���͵�ַ
	MAX30102_IIC_Wait_Ack();		 
	MAX30102_IIC_Start();  	 	   
	MAX30102_IIC_Send_Byte(daddr|0X01);//�������ģʽ			   
	MAX30102_IIC_Wait_Ack();	 
    *data = MAX30102_IIC_Read_Byte(0);		   
    MAX30102_IIC_Stop();//����һ��ֹͣ����	    
}
 
void MAX30102_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data)
{				   	  	    																 
    MAX30102_IIC_Start();  
	
	MAX30102_IIC_Send_Byte(daddr);	    //����д����
	MAX30102_IIC_Wait_Ack();
	MAX30102_IIC_Send_Byte(addr);//���͵�ַ
	MAX30102_IIC_Wait_Ack();	   	 										  		   
	MAX30102_IIC_Send_Byte(data);     //�����ֽ�							   
	MAX30102_IIC_Wait_Ack();  		    	   
    MAX30102_IIC_Stop();//����һ��ֹͣ���� 
	Delay_ms(10);	 
}

 
 
 
 
 
 
const uint16_t auw_hamm[31]={ 41,    276,    512,    276,     41 }; //Hamm=  long16(512* hamming(5)');
//uch_spo2_table is computed as  -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 ;
const uint8_t uch_spo2_table[184]={ 95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, 
                            99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
                            100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97, 
                            97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, 
                            90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81, 
                            80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67, 
                            66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 
                            49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, 
                            28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5, 
                            3, 2, 1 } ;
static  int32_t an_dx[ BUFFER_SIZE-MA4_SIZE]; // delta
static  int32_t an_x[ BUFFER_SIZE]; //ir
static  int32_t an_y[ BUFFER_SIZE]; //red
 
void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer,  int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, 
                              int32_t *pn_heart_rate, int8_t  *pch_hr_valid)
/**
* \brief        Calculate the heart rate and SpO2 level
* \par          Details
*               By detecting  peaks of PPG cycle and corresponding AC/DC of red/infra-red signal, the ratio for the SPO2 is computed.
*               Since this algorithm is aiming for Arm M0/M3. formaula for SPO2 did not achieve the accuracy due to register overflow.
*               Thus, accurate SPO2 is precalculated and save longo uch_spo2_table[] per each ratio.
*
* \param[in]    *pun_ir_buffer           - IR sensor data buffer
* \param[in]    n_ir_buffer_length      - IR sensor data buffer length
* \param[in]    *pun_red_buffer          - Red sensor data buffer
* \param[out]    *pn_spo2                - Calculated SpO2 value
* \param[out]    *pch_spo2_valid         - 1 if the calculated SpO2 value is valid
* \param[out]    *pn_heart_rate          - Calculated heart rate value
* \param[out]    *pch_hr_valid           - 1 if the calculated heart rate value is valid
*
* \retval       None
*/
{
    uint32_t un_ir_mean ,un_only_once ;
    int32_t k ,n_i_ratio_count;
    int32_t i, s, m, n_exact_ir_valley_locs_count ,n_middle_idx;
    int32_t n_th1, n_npks,n_c_min;      
    int32_t an_ir_valley_locs[15] ;
    int32_t an_exact_ir_valley_locs[15] ;
    int32_t an_dx_peak_locs[15] ;
    int32_t n_peak_interval_sum;
    
    int32_t n_y_ac, n_x_ac;
    int32_t n_spo2_calc; 
    int32_t n_y_dc_max, n_x_dc_max; 
    int32_t n_y_dc_max_idx, n_x_dc_max_idx; 
    int32_t an_ratio[5],n_ratio_average; 
    int32_t n_nume,  n_denom ;
    // remove DC of ir signal    
    un_ir_mean =0; 
    for (k=0 ; k<n_ir_buffer_length ; k++ ) un_ir_mean += pun_ir_buffer[k] ;
    un_ir_mean =un_ir_mean/n_ir_buffer_length ;
    for (k=0 ; k<n_ir_buffer_length ; k++ )  an_x[k] =  pun_ir_buffer[k] - un_ir_mean ; 
    
    // 4 pt Moving Average
    for(k=0; k< BUFFER_SIZE-MA4_SIZE; k++){
        n_denom= ( an_x[k]+an_x[k+1]+ an_x[k+2]+ an_x[k+3]);
        an_x[k]=  n_denom/(int32_t)4; 
    }
 
    // get difference of smoothed IR signal
    
    for( k=0; k<BUFFER_SIZE-MA4_SIZE-1;  k++)
        an_dx[k]= (an_x[k+1]- an_x[k]);
 
    // 2-pt Moving Average to an_dx
    for(k=0; k< BUFFER_SIZE-MA4_SIZE-2; k++){
        an_dx[k] =  ( an_dx[k]+an_dx[k+1])/2 ;
    }
    
    // hamming window
    // flip wave form so that we can detect valley with peak detector
    for ( i=0 ; i<BUFFER_SIZE-HAMMING_SIZE-MA4_SIZE-2 ;i++){
        s= 0;
        for( k=i; k<i+ HAMMING_SIZE ;k++){
            s -= an_dx[k] *auw_hamm[k-i] ; 
                     }
        an_dx[i]= s/ (int32_t)1146; // divide by sum of auw_hamm 
    }
 
 
    n_th1=0; // threshold calculation
    for ( k=0 ; k<BUFFER_SIZE-HAMMING_SIZE ;k++){
        n_th1 += ((an_dx[k]>0)? an_dx[k] : ((int32_t)0-an_dx[k])) ;
    }
    n_th1= n_th1/ ( BUFFER_SIZE-HAMMING_SIZE);
    // peak location is acutally index for sharpest location of raw signal since we flipped the signal         
    maxim_find_peaks( an_dx_peak_locs, &n_npks, an_dx, BUFFER_SIZE-HAMMING_SIZE, n_th1, 8, 5 );//peak_height, peak_distance, max_num_peaks 
 
    n_peak_interval_sum =0;
    if (n_npks>=2){
        for (k=1; k<n_npks; k++)
            n_peak_interval_sum += (an_dx_peak_locs[k]-an_dx_peak_locs[k -1]);
        n_peak_interval_sum=n_peak_interval_sum/(n_npks-1);
        *pn_heart_rate=(int32_t)(6000/n_peak_interval_sum);// beats per minutes
        *pch_hr_valid  = 1;
    }
    else  {
        *pn_heart_rate = -999;
        *pch_hr_valid  = 0;
    }
            
    for ( k=0 ; k<n_npks ;k++)
        an_ir_valley_locs[k]=an_dx_peak_locs[k]+HAMMING_SIZE/2; 
 
 
    // raw value : RED(=y) and IR(=X)
    // we need to assess DC and AC value of ir and red PPG. 
    for (k=0 ; k<n_ir_buffer_length ; k++ )  {
        an_x[k] =  pun_ir_buffer[k] ; 
        an_y[k] =  pun_red_buffer[k] ; 
    }
 
    // find precise min near an_ir_valley_locs
    n_exact_ir_valley_locs_count =0; 
    for(k=0 ; k<n_npks ;k++){
        un_only_once =1;
        m=an_ir_valley_locs[k];
        n_c_min= 16777216;//2^24;
        if (m+5 <  BUFFER_SIZE-HAMMING_SIZE  && m-5 >0){
            for(i= m-5;i<m+5; i++)
                if (an_x[i]<n_c_min){
                    if (un_only_once >0){
                       un_only_once =0;
                   } 
                   n_c_min= an_x[i] ;
                   an_exact_ir_valley_locs[k]=i;
                }
            if (un_only_once ==0)
                n_exact_ir_valley_locs_count ++ ;
        }
    }
    if (n_exact_ir_valley_locs_count <2 ){
       *pn_spo2 =  -999 ; // do not use SPO2 since signal ratio is out of range
       *pch_spo2_valid  = 0; 
       return;
    }
    // 4 pt MA
    for(k=0; k< BUFFER_SIZE-MA4_SIZE; k++){
        an_x[k]=( an_x[k]+an_x[k+1]+ an_x[k+2]+ an_x[k+3])/(int32_t)4;
        an_y[k]=( an_y[k]+an_y[k+1]+ an_y[k+2]+ an_y[k+3])/(int32_t)4;
    }
 
    //using an_exact_ir_valley_locs , find ir-red DC andir-red AC for SPO2 calibration ratio
    //finding AC/DC maximum of raw ir * red between two valley locations
    n_ratio_average =0; 
    n_i_ratio_count =0; 
    
    for(k=0; k< 5; k++) an_ratio[k]=0;
    for (k=0; k< n_exact_ir_valley_locs_count; k++){
        if (an_exact_ir_valley_locs[k] > BUFFER_SIZE ){             
            *pn_spo2 =  -999 ; // do not use SPO2 since valley loc is out of range
            *pch_spo2_valid  = 0; 
            return;
        }
    }
    // find max between two valley locations 
    // and use ratio betwen AC compoent of Ir & Red and DC compoent of Ir & Red for SPO2 
 
    for (k=0; k< n_exact_ir_valley_locs_count-1; k++){
        n_y_dc_max= -16777216 ; 
        n_x_dc_max= - 16777216; 
        if (an_exact_ir_valley_locs[k+1]-an_exact_ir_valley_locs[k] >10){
            for (i=an_exact_ir_valley_locs[k]; i< an_exact_ir_valley_locs[k+1]; i++){
                if (an_x[i]> n_x_dc_max) {n_x_dc_max =an_x[i];n_x_dc_max_idx =i; }
                if (an_y[i]> n_y_dc_max) {n_y_dc_max =an_y[i];n_y_dc_max_idx=i;}
            }
            n_y_ac= (an_y[an_exact_ir_valley_locs[k+1]] - an_y[an_exact_ir_valley_locs[k] ] )*(n_y_dc_max_idx -an_exact_ir_valley_locs[k]); //red
            n_y_ac=  an_y[an_exact_ir_valley_locs[k]] + n_y_ac/ (an_exact_ir_valley_locs[k+1] - an_exact_ir_valley_locs[k])  ; 
        
        
            n_y_ac=  an_y[n_y_dc_max_idx] - n_y_ac;    // subracting linear DC compoenents from raw 
            n_x_ac= (an_x[an_exact_ir_valley_locs[k+1]] - an_x[an_exact_ir_valley_locs[k] ] )*(n_x_dc_max_idx -an_exact_ir_valley_locs[k]); // ir
            n_x_ac=  an_x[an_exact_ir_valley_locs[k]] + n_x_ac/ (an_exact_ir_valley_locs[k+1] - an_exact_ir_valley_locs[k]); 
            n_x_ac=  an_x[n_y_dc_max_idx] - n_x_ac;      // subracting linear DC compoenents from raw 
            n_nume=( n_y_ac *n_x_dc_max)>>7 ; //prepare X100 to preserve floating value
            n_denom= ( n_x_ac *n_y_dc_max)>>7;
            if (n_denom>0  && n_i_ratio_count <5 &&  n_nume != 0)
            {   
                an_ratio[n_i_ratio_count]= (n_nume*20)/n_denom ; //formular is ( n_y_ac *n_x_dc_max) / ( n_x_ac *n_y_dc_max) ;  ///*************************n_numeԭ����*100************************//
                n_i_ratio_count++;
            }
        }
    }
 
    maxim_sort_ascend(an_ratio, n_i_ratio_count);
    n_middle_idx= n_i_ratio_count/2;
 
    if (n_middle_idx >1)
        n_ratio_average =( an_ratio[n_middle_idx-1] +an_ratio[n_middle_idx])/2; // use median
    else
        n_ratio_average = an_ratio[n_middle_idx ];
 
    if( n_ratio_average>2 && n_ratio_average <184){
        n_spo2_calc= uch_spo2_table[n_ratio_average] ;
        *pn_spo2 = n_spo2_calc ;
        *pch_spo2_valid  = 1;//  float_SPO2 =  -45.060*n_ratio_average* n_ratio_average/10000 + 30.354 *n_ratio_average/100 + 94.845 ;  // for comparison with table
    }
    else{
        *pn_spo2 =  -999 ; // do not use SPO2 since signal ratio is out of range
        *pch_spo2_valid  = 0; 
    }
}
 
 
void maxim_find_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num)
/**
* \brief        Find peaks
* \par          Details
*               Find at most MAX_NUM peaks above MIN_HEIGHT separated by at least MIN_DISTANCE
*
* \retval       None
*/
{
    maxim_peaks_above_min_height( pn_locs, pn_npks, pn_x, n_size, n_min_height );
    maxim_remove_close_peaks( pn_locs, pn_npks, pn_x, n_min_distance );
    *pn_npks = min( *pn_npks, n_max_num );
}
 
void maxim_peaks_above_min_height(int32_t *pn_locs, int32_t *pn_npks, int32_t  *pn_x, int32_t n_size, int32_t n_min_height)
/**
* \brief        Find peaks above n_min_height
* \par          Details
*               Find all peaks above MIN_HEIGHT
*
* \retval       None
*/
{
    int32_t i = 1, n_width;
    *pn_npks = 0;
    
    while (i < n_size-1){
        if (pn_x[i] > n_min_height && pn_x[i] > pn_x[i-1]){            // find left edge of potential peaks
            n_width = 1;
            while (i+n_width < n_size && pn_x[i] == pn_x[i+n_width])    // find flat peaks
                n_width++;
            if (pn_x[i] > pn_x[i+n_width] && (*pn_npks) < 15 ){                            // find right edge of peaks
                pn_locs[(*pn_npks)++] = i;        
                // for flat peaks, peak location is left edge
                i += n_width+1;
            }
            else
                i += n_width;
        }
        else
            i++;
    }
}
 
 
void maxim_remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance)
/**
* \brief        Remove peaks
* \par          Details
*               Remove peaks separated by less than MIN_DISTANCE
*
* \retval       None
*/
{
    
    int32_t i, j, n_old_npks, n_dist;
    
    /* Order peaks from large to small */
    maxim_sort_indices_descend( pn_x, pn_locs, *pn_npks );
 
    for ( i = -1; i < *pn_npks; i++ ){
        n_old_npks = *pn_npks;
        *pn_npks = i+1;
        for ( j = i+1; j < n_old_npks; j++ ){
            n_dist =  pn_locs[j] - ( i == -1 ? -1 : pn_locs[i] ); // lag-zero peak of autocorr is at index -1
            if ( n_dist > n_min_distance || n_dist < -n_min_distance )
                pn_locs[(*pn_npks)++] = pn_locs[j];
        }
    }
 
    // Resort indices longo ascending order
    maxim_sort_ascend( pn_locs, *pn_npks );
}
 
void maxim_sort_ascend(int32_t *pn_x,int32_t n_size) 
/**
* \brief        Sort array
* \par          Details
*               Sort array in ascending order (insertion sort algorithm)
*
* \retval       None
*/
{
    int32_t i, j, n_temp;
    for (i = 1; i < n_size; i++) {
        n_temp = pn_x[i];
        for (j = i; j > 0 && n_temp < pn_x[j-1]; j--)
            pn_x[j] = pn_x[j-1];
        pn_x[j] = n_temp;
    }
}
 
void maxim_sort_indices_descend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size)
/**
* \brief        Sort indices
* \par          Details
*               Sort indices according to descending order (insertion sort algorithm)
*
* \retval       None
*/ 
{
    int32_t i, j, n_temp;
    for (i = 1; i < n_size; i++) {
        n_temp = pn_indx[i];
        for (j = i; j > 0 && pn_x[n_temp] > pn_x[pn_indx[j-1]]; j--)
            pn_indx[j] = pn_indx[j-1];
        pn_indx[j] = n_temp;
    }
}

void max_init(void)
{
	un_min=0x3FFFF;
	un_max=0;
	
 
	n_ir_buffer_length=500; //����������Ϊ100���ɴ洢��100sps���е�5������
	//��ȡǰ500����������ȷ���źŷ�Χ
	for(i=0;i<n_ir_buffer_length;i++)
	{
			while(MAX30102_INT==1);   //�ȴ���ֱ���ж����Ŷ���
			
			max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
			aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // ��ֵ�ϲ��õ�ʵ������
			aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   	 // ��ֵ�ϲ��õ�ʵ������
					
			if(un_min>aun_red_buffer[i])
					un_min=aun_red_buffer[i];    //���¼�����Сֵ
			if(un_max<aun_red_buffer[i])
					un_max=aun_red_buffer[i];    //���¼������ֵ
	}
	un_prev_data=aun_red_buffer[i];
	
	//����ǰ500��������ǰ5���������������ʺ�Ѫ�����Ͷ�
	maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
}

void max_show(void)
{
			i=0;
			un_min=0x3FFFF;
			un_max=0;
		//��ȥǰ100��������������400�������Ƶ���������100~500����������λ��0~400
			for(i=100;i<500;i++)
			{
					aun_red_buffer[i-100]=aun_red_buffer[i];	//��100-500����������λ��0-400
					aun_ir_buffer[i-100]=aun_ir_buffer[i];		//��100-500����������λ��0-400
					
					//update the signal min and max
					if(un_min>aun_red_buffer[i])			//Ѱ����λ��0-400�е���Сֵ
						un_min=aun_red_buffer[i];
					if(un_max<aun_red_buffer[i])			//Ѱ����λ��0-400�е����ֵ
						un_max=aun_red_buffer[i];
			}
			
			//�ڼ�������ǰȡ100��������ȡ�����ݷ���400-500����������
			for(i=400;i<500;i++)
			{
					un_prev_data=aun_red_buffer[i-1];	//�ڼ�������ǰȡ100��������ȡ�����ݷ���400-500����������
					while(MAX30102_INT==1);
					max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);		//��ȡ���������ݣ���ֵ��temp��
					aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    //��ֵ�ϲ��õ�ʵ�����֣�����400-500Ϊ�¶�ȡ����
					aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   	//��ֵ�ϲ��õ�ʵ�����֣�����400-500Ϊ�¶�ȡ����
					if(aun_red_buffer[i]>un_prev_data)		//���»�ȡ��һ����ֵ����һ����ֵ�Ա�
					{
							f_temp=aun_red_buffer[i]-un_prev_data;
							f_temp/=(un_max-un_min);
							f_temp*=MAX_BRIGHTNESS;			//��ʽ���������ߣ�=������ֵ-����ֵ��/�����ֵ-��Сֵ��*255
							n_brightness-=(int)f_temp;
							if(n_brightness<0)
									n_brightness=0;
					}
					else
					{
							f_temp=un_prev_data-aun_red_buffer[i];
							f_temp/=(un_max-un_min);
							f_temp*=MAX_BRIGHTNESS;			//��ʽ���������ߣ�=������ֵ-����ֵ��/�����ֵ-��Сֵ��*255
							n_brightness+=(int)f_temp;
							if(n_brightness>MAX_BRIGHTNESS)
									n_brightness=MAX_BRIGHTNESS;
					}
			//ͨ��UART�������ͼ��������͵��ն˳���
			if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
			{
				dis_hr = n_heart_rate;
				dis_spo2 = n_sp02;
			}
			else
			{
				dis_hr = 0;
				dis_spo2 = 0;
			}
		}
		maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		
		//MAX30102
//		if(dis_hr==0)
//			OLED_ShowNum(47,0,0,3,16,1);
//		else
//		OLED_ShowNum(47,0,dis_hr-20,3,16,1);
//		
//		OLED_ShowNum(47,16,dis_spo2,2,16,1);
		
		if(dis_hr-20>40 && dis_spo2>=80)
		{
			printf("\r\n����= %d BPM Ѫ��= %d\r\n ",dis_hr-20,dis_spo2);
			dis_hr_upload = dis_hr - 20;
			dis_spo2_upload = dis_spo2;
		}
		if(dis_spo2_upload>100||dis_spo2_upload<0)
				dis_spo2_upload=100;
		//Serial_SendString("\r\n");
}
