#ifndef _GPS_//Ԥ����
#define _GPS_
#include "sys.h"
#include "stdio.h"
#include "string.h"
#include "malloc.h"
#include "oled.h"
#include "stdlib.h"
#include "stdbool.h"

//#define FALSE 0
//#define TRUE  1


typedef struct Data
{
	char GPS_Buffer[200];//���ݽ���	
	bool isGetData;//���������Ƿ����
	char *UTCTime;//ʱ���
	char *latitude;//γ��
	char *N_S;//�ϱ�
	char *longitude;//����
	char *E_W;//����
	char *speed;
  char *course;
	char *date;
	bool isParseData;//�Ƿ�������
	bool isUsefull;//�Ƿ�Ϊ��Ч����λ
}Data;





void parseGpsBuffer(void);
void printfGpsBuffer(void);
#endif
