#ifndef __ALARM_H
#define __ALARM_H

#include "sys.h"

/************************************************************
 * 报警模块 - 蜂鸣器和LED报警灯
 * 蜂鸣器: PA4 (高电平触发)
 * LED灯:  PA5 (高电平点亮)
 ************************************************************/

// 蜂鸣器控制宏
#define BUZZER_ON()     GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define BUZZER_OFF()    GPIO_ResetBits(GPIOA, GPIO_Pin_4)

// LED报警灯控制宏
#define LED_ALARM_ON()  GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define LED_ALARM_OFF() GPIO_ResetBits(GPIOA, GPIO_Pin_5)

// 函数声明
void Alarm_Init(void);
void Alarm_On(void);
void Alarm_Off(void);
void Alarm_Beep_Short(void);
void Alarm_Full_Warning(void);

#endif
