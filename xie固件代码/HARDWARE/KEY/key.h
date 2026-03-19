#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

/************************************************************
 * 按键模块 - 8个独立按键
 *
 * SW0  - 系统复位 (硬件复位，连接NRST，不占用GPIO)
 * SW2  - PB12 - 自动/手动模式切换
 * SW3  - PB13 - 入口道闸控制
 * SW4  - PB14 - 出口道闸控制
 * SW5  - PB15 - 紧急放行
 * SW6  - PA11 - 菜单/确认
 * SW7  - PA12 - 增加车位
 * SW8  - PA15 - 减少车位
 ************************************************************/

// 按键端口定义
#define KEY_SW2_GPIO_CLK        RCC_APB2Periph_GPIOB
#define KEY_SW2_PORT            GPIOB
#define KEY_SW2_PIN             GPIO_Pin_12
#define KEY_SW2                 PBin(12)

#define KEY_SW3_GPIO_CLK        RCC_APB2Periph_GPIOB
#define KEY_SW3_PORT            GPIOB
#define KEY_SW3_PIN             GPIO_Pin_13
#define KEY_SW3                 PBin(13)

#define KEY_SW4_GPIO_CLK        RCC_APB2Periph_GPIOB
#define KEY_SW4_PORT            GPIOB
#define KEY_SW4_PIN             GPIO_Pin_14
#define KEY_SW4                 PBin(14)

#define KEY_SW5_GPIO_CLK        RCC_APB2Periph_GPIOB
#define KEY_SW5_PORT            GPIOB
#define KEY_SW5_PIN             GPIO_Pin_15
#define KEY_SW5                 PBin(15)

#define KEY_SW6_GPIO_CLK        RCC_APB2Periph_GPIOA
#define KEY_SW6_PORT            GPIOA
#define KEY_SW6_PIN             GPIO_Pin_11
#define KEY_SW6                 PAin(11)

#define KEY_SW7_GPIO_CLK        RCC_APB2Periph_GPIOA
#define KEY_SW7_PORT            GPIOA
#define KEY_SW7_PIN             GPIO_Pin_12
#define KEY_SW7                 PAin(12)

#define KEY_SW8_GPIO_CLK        RCC_APB2Periph_GPIOA
#define KEY_SW8_PORT            GPIOA
#define KEY_SW8_PIN             GPIO_Pin_15
#define KEY_SW8                 PAin(15)

// 按键值定义
#define KEY_NONE                0
#define KEY_MODE_SWITCH         2   // SW2 - 模式切换
#define KEY_ENTRANCE_GATE       3   // SW3 - 入口道闸
#define KEY_EXIT_GATE           4   // SW4 - 出口道闸
#define KEY_EMERGENCY           5   // SW5 - 紧急放行
#define KEY_MENU                6   // SW6 - 菜单/确认
#define KEY_SPACE_ADD           7   // SW7 - 增加车位
#define KEY_SPACE_SUB           8   // SW8 - 减少车位

// 函数声明
void KEY_Init(void);
uint8_t KEY_Scan(uint8_t mode);

#endif
