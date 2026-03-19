#ifndef __MOTOR_DUOJI_H
#define __MOTOR_DUOJI_H

#include "sys.h"

/************************************************************
 * 舵机控制模块 - SG90 舵机
 * 用于控制入口和出口道闸
 *
 * 入口道闸: TIM2_CH3 (PA2)
 * 出口道闸: TIM2_CH4 (PA3)
 ************************************************************/

// 入口道闸舵机 - TIM2_CH3 (PA2)
#define SERVO_ENTRANCE_TIM              TIM2
#define SERVO_ENTRANCE_TIM_CLK          RCC_APB1Periph_TIM2
#define SERVO_ENTRANCE_GPIO_CLK         RCC_APB2Periph_GPIOA
#define SERVO_ENTRANCE_PORT             GPIOA
#define SERVO_ENTRANCE_PIN              GPIO_Pin_2

// 出口道闸舵机 - TIM2_CH4 (PA3)
#define SERVO_EXIT_TIM                  TIM2
#define SERVO_EXIT_TIM_CLK              RCC_APB1Periph_TIM2
#define SERVO_EXIT_GPIO_CLK             RCC_APB2Periph_GPIOA
#define SERVO_EXIT_PORT                 GPIOA
#define SERVO_EXIT_PIN                  GPIO_Pin_3

// 舵机角度控制参数 (PWM脉宽值)
// SG90: 周期20ms(50Hz), 脉宽0.5ms~2.5ms对应0°~180°
// 72MHz / 72 / 20000 = 50Hz
// 0.5ms = 500, 1.5ms = 1500, 2.5ms = 2500
#define SERVO_ANGLE_0       500     // 0度 (道闸水平关闭)
#define SERVO_ANGLE_90      1500    // 90度 (道闸垂直开启)
#define SERVO_ANGLE_180     2500    // 180度

// 函数声明
void Servo_Init(void);
void Servo_Entrance_SetAngle(uint16_t angle);
void Servo_Exit_SetAngle(uint16_t angle);
void Servo_Entrance_Open(void);
void Servo_Entrance_Close(void);
void Servo_Exit_Open(void);
void Servo_Exit_Close(void);

#endif
