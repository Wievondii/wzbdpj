#ifndef __MOTOR_DUOJI_H__
#define __MOTOR_DUOJI_H__

/**********************************
 * 头文件
 **********************************/
#include "sys.h"

/**********************************
 * PIN 引脚定义 (修改后 - 单舵机版本)
 **********************************/

// TIM2 配置 - 只保留 1 个舵机接口 (SG90_1)
// JS 接口定义：
//   JS1 (PA4) - 红外满溢检测
//   JS2 (PA5) - HC-SR04 ECHO 输入
//   JS3 (PA6) - 舵机控制
//   SG90_1 (PA0) - 蜂鸣器

#define MOTOR_DUOJI_TIM_CLK_ENABLE          RCC_APB1Periph_TIM3       // 定时器 3 时钟
#define MOTOR_DUOJI_TIM                     TIM3
#define MOTOR_DUOJI_GPIO_CLK_ENABLE         RCC_APB2Periph_GPIOA
#define MOTOR_DUOJI_PORT                    GPIOA
#define MOTOR_DUOJI_PIN                     GPIO_Pin_6      // TIM3_CH1 - 舵机信号
#define MOTOR_DUOJI_PIN_2                   GPIO_Pin_7      // 保留

// TIM1 配置已禁用 (原 SG90_3/SG90_4)
// #define MOTOR_DUOJI_TIM_CLK_ENABLE        RCC_APB2Periph_TIM1
// #define MOTOR_DUOJI_TIM                   TIM1
// #define MOTOR_DUOJI_PIN                   GPIO_Pin_8      // 禁用
// #define MOTOR_DUOJI_PIN_2                 GPIO_Pin_11     // 禁用

#define MOTOR_DUOJI2_TIM_CLK_ENABLE         RCC_APB1Periph_TIM3
#define MOTOR_DUOJI2_TIM                    TIM3
#define MOTOR_DUOJI2_GPIO_CLK_ENABLE        RCC_APB2Periph_GPIOA
#define MOTOR_DUOJI2_PORT                   GPIOA
#define MOTOR_DUOJI2_PIN                    GPIO_Pin_6      // TIM3_CH1 - 舵机信号
#define MOTOR_DUOJI2_PIN_2                  GPIO_Pin_7

/**********************************
 * 函数声明
 **********************************/
void Motor_Duoji_Init(void);                // 舵机初始化 (TIM3_CH1 - PA6)
void Motor_Duoji2_Init(void);               // 保留函数 (实际禁用)
void Motor_Duoji_SetCompare(u16 compare);   // 设置 TIM3_CH1 比较值

#endif
