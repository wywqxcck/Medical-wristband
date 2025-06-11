#ifndef __APP_H
#define __APP_H

#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "Buzzer.h"
#include "usart.h"
#include "Key.h"
#include "stdbool.h"
#include "malloc.h"
#include "usart2.h"
#include "GPS.h"
#include "math.h"
#include "usart3.h"
#include "Timer.h"
#include "max30102.h"
#include "MPU6050.h"
#include <math.h>
#include <stdio.h>
#include "LED.h"

void app_init(void);
void app_run(void);


#endif
