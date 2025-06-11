#include "app.h"

#define SHAKE_THRESHOLD 2000
#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

uint8_t hr_limit=100;
bool model = true;
float ppm;
uint8_t detect_fall_flage = 0;
uint8_t oled_stat=0;

extern double latitude,longitude;
extern u32 dis_hr_upload,dis_spo2_upload;
extern u32 UTCTime;
extern u32 date;
int day,month,year,hours,minutes,seconds;

void oled_show(void);
int detect_fall(void);
void key_deal(void);

void app_init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
		
	uart_init(115200);
	usart2_init(9600);
	usart3_init(9600);
	my_mem_init(SRAMIN);

	MAX30102_Init();
	max_init();
	Shake_Init();
	OLED_Init();
	MPU6050_Init();
	Key_Init();
	Shake_OFF();
	LED_Init();
}

void app_run(void)
{
	max_show();
	oled_show();
	key_deal();
	LED1_Turn();
	Delay_ms(100);
}

void oled_show(void)
{
	if(!oled_stat)
	{
		OLED_ShowString(0,0,"HR:",16);
		OLED_ShowString(0,2,"Sp:",16);
		OLED_ShowNum(25,0,dis_hr_upload,3,16);
		OLED_ShowNum(25,2,dis_spo2_upload,3,16);
		
		char arr[100];
		sprintf(arr,"%.2f",latitude);
		OLED_ShowString(0,4,"lati:",16);
		OLED_ShowString(40,4,(char*)arr,16);
		sprintf(arr,"%.2f",longitude);
		OLED_ShowString(0,6,"long:",16);	
		OLED_ShowString(40,6,(char*)arr,16);	
		
		detect_fall_flage=detect_fall();	
	}
	else
	{
    hours=UTCTime/10000;
		hours+=8;
		minutes=(UTCTime / 100) % 100;
		seconds=UTCTime % 100;
		
		OLED_ShowNum(0,0,hours,2,16);
		OLED_ShowChar(20,0,':',16);
		OLED_ShowNum(30,0,minutes,2,16);
		OLED_ShowChar(50,0,':',16);
		OLED_ShowNum(60,0,seconds,2,16);
	}
	
	if(dis_hr_upload>hr_limit || detect_fall_flage==1)
	{
		Shake_ON();
	}
	else
		Shake_OFF();
}

// 计算倾斜角度（简单示例，仅使用加速度计数据）
void calculate_angles(int16_t AccX, int16_t AccY, int16_t AccZ, float *pitch, float *roll) 
{
	*pitch = atan2(AccX, sqrt(AccY * AccY + AccZ * AccZ)) * 180 / M_PI;
	*roll = atan2(AccY, sqrt(AccX * AccX + AccZ * AccZ)) * 180 / M_PI;
}

// 跌倒检测函数
int detect_fall(void)
{
	int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
	float pitch, roll;
	const float fall_threshold = 40.0; // 跌倒角度阈值，可根据实际情况调整

	// 获取 MPU6050 数据
	MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

	// 计算倾斜角度
	calculate_angles(AccX, AccY, AccZ, &pitch, &roll);

	// 判断是否跌倒
	if (fabs(pitch) > fall_threshold || fabs(roll) > fall_threshold) 
	{
		return 1; // 跌倒
	} 
	else 
	{
		return 0; // 未跌倒
	}
}

void key_deal(void)
{
	uint8_t keynum=Key_GetNum();
	if(keynum==1)
	{
		OLED_Clear();
		if(oled_stat)
			oled_stat=0;
		else
			oled_stat=1;
	}
}
