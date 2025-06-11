#include "GPS.h"


double latitude = 113.2254,longitude = 34.77854;
char * pEnd;
u32 UTCTime;
u32 date;
const short int USART2_MAX_RECV_LEN = 200;
char USAR2_RX_BUF[USART2_MAX_RECV_LEN];
const short int GPS_Buffer_Length = 200;
struct Data Save_Data;

void parseGpsBuffer(void){
	char *subString;
	char *subStringNext;
	int i = 0;
	if(Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
//		printf("*****************\r\n");
//		printf("%s",Save_Data.GPS_Buffer);
		
		for(i = 0; i <= 6; i++){
			if(i == 0){
				if((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
				{}
//					printf("��������");
				}
				else{
					subString++;//������������ж��ŵ���һλ
					if((subStringNext = strstr(subString, ",")) != NULL)
					{
						char usefullBuffer[2];
						switch(i){
							case 1: 
								//����subStringNext��subString���׵�ַ�����ȷ��ָ�뿪�ٿռ�Ĵ�С���Է�ָ�벻�Ϸ���
								Save_Data.UTCTime = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
								memcpy(Save_Data.UTCTime, subString, subStringNext - subString); 
								break;
							case 2: memcpy(usefullBuffer, subString, subStringNext - subString); break;
							case 3: Save_Data.latitude = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
								memcpy(Save_Data.latitude, subString, subStringNext - subString); 
								break;
							case 4: Save_Data.N_S = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
								memcpy(Save_Data.N_S, subString, subStringNext - subString); 
								break;
							case 5: Save_Data.longitude = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
								memcpy(Save_Data.longitude, subString, subStringNext - subString); 
								break;
							case 6: Save_Data.E_W = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
								memcpy(Save_Data.E_W, subString, subStringNext - subString); 
								break;
//							case 7: Save_Data.speed = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
//								memcpy(Save_Data.speed, subString, subStringNext - subString); 
//								break;
//							case 8: Save_Data.course = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
//								memcpy(Save_Data.course, subString, subStringNext - subString); 
//								break;
//							case 9: Save_Data.date = (char *)mymalloc(SRAMIN,(subStringNext - subString)*sizeof(char));
//								memcpy(Save_Data.date, subString, subStringNext - subString); 
//								break;
							default: break;
						}
						subString = subStringNext;
						Save_Data.isParseData = true;
						if(usefullBuffer[0] == 'A')
							Save_Data.isUsefull = true;
						else if(usefullBuffer[0] == 'V')
							Save_Data.isUsefull = false;
					}
					else{
//							printf("��������2");
					}
				}
			}
		}
}


void printfGpsBuffer(void){
	if(Save_Data.isParseData){
		Save_Data.isParseData = false;
//		printf("Save_Data.UTCTime = %s\r\n", Save_Data.UTCTime);//��ӡ����
		UTCTime = atoi(Save_Data.UTCTime);
		date = atoi(Save_Data.date);
		myfree(SRAMIN,Save_Data.UTCTime);//�ͷſռ�
		
		if(Save_Data.isUsefull){
			Save_Data.isUsefull = false;
//			printf("Save_Data.latitude = %s\r\n", Save_Data.latitude);
			latitude = strtod(Save_Data.latitude,&pEnd);
			myfree(SRAMIN,Save_Data.latitude);
//			printf("Save_Data.N_S = %s\r\n", Save_Data.N_S);
			myfree(SRAMIN,Save_Data.N_S);
//			printf("Save_Data.longitude = %s\r\n", Save_Data.longitude);
			longitude = strtod(Save_Data.longitude,&pEnd);
			myfree(SRAMIN,Save_Data.longitude);
//			printf("Save_Data.E_W = %s\r\n", Save_Data.E_W);
			myfree(SRAMIN,Save_Data.E_W);
			UTCTime = strtod(Save_Data.UTCTime,&pEnd);
			myfree(SRAMIN,Save_Data.UTCTime);

		}
//		else
//		{
//			printf("GPS DATA is not usefull!\r\n");
//		}
	}
}

