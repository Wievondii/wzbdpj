#ifndef __GPIO_H
#define __GPIO_H

#include "sys.h"

/************************************************************
 * 智能停车场系统 GPIO 引脚定义
 * 基于 STM32F103C8T6
 ************************************************************/

// 车位检测红外传感器 (反射式红外避障模块)
// 车位空闲时输出高电平，有车时输出低电平
#define IR_SPACE1_GPIO_CLK      RCC_APB2Periph_GPIOB
#define IR_SPACE1_PORT          GPIOB
#define IR_SPACE1_PIN           GPIO_Pin_2
#define IR_SPACE1               PBin(2)     // JS5 - PB2

#define IR_SPACE2_GPIO_CLK      RCC_APB2Periph_GPIOB
#define IR_SPACE2_PORT          GPIOB
#define IR_SPACE2_PIN           GPIO_Pin_4
#define IR_SPACE2               PBin(4)     // JS8 - PB4

#define IR_SPACE3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define IR_SPACE3_PORT          GPIOB
#define IR_SPACE3_PIN           GPIO_Pin_6
#define IR_SPACE3               PBin(6)     // JS10 - PB6

// 入口车辆检测红外传感器
#define IR_ENTRANCE_GPIO_CLK    RCC_APB2Periph_GPIOA
#define IR_ENTRANCE_PORT        GPIOA
#define IR_ENTRANCE_PIN         GPIO_Pin_0
#define IR_ENTRANCE             PAin(0)     // JS11 - PA0

// 出口车辆检测红外传感器
#define IR_EXIT_GPIO_CLK        RCC_APB2Periph_GPIOA
#define IR_EXIT_PORT            GPIOA
#define IR_EXIT_PIN             GPIO_Pin_1
#define IR_EXIT                 PAin(1)     // JS12 - PA1

// 蜂鸣器控制 (高电平触发)
#define BUZZER_GPIO_CLK         RCC_APB2Periph_GPIOA
#define BUZZER_PORT             GPIOA
#define BUZZER_PIN              GPIO_Pin_4
#define BUZZER                  PAout(4)    // PA4

// LED 报警灯 (高电平点亮)
#define LED_ALARM_GPIO_CLK      RCC_APB2Periph_GPIOA
#define LED_ALARM_PORT          GPIOA
#define LED_ALARM_PIN           GPIO_Pin_5
#define LED_ALARM               PAout(5)    // PA5

// 函数声明
void GPIO_Config(void);

#endif
