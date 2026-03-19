/**********************************
 * 头文件
 **********************************/
#include "motor_duoji.h"
#include "delay.h"

// PWM 值对应舵机角度 (1750-1950 对应 0-180 度)
uint16_t motor_duoji_cout = 1850;   // 初始值 90 度


/****
******* 舵机初始化 (TIM3 - PA6)
*****/
void Motor_Duoji_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启定时器 3 和 GPIOA 时钟
    RCC_APB1PeriphClockCmd(MOTOR_DUOJI2_TIM_CLK_ENABLE, ENABLE);
    RCC_APB2PeriphClockCmd(MOTOR_DUOJI2_GPIO_CLK_ENABLE, ENABLE);

    // 配置 PA6 为复用推挽输出 (TIM3_CH1)
    GPIO_InitStructure.GPIO_Pin = MOTOR_DUOJI2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_DUOJI2_PORT, &GPIO_InitStructure);

    // TIM3 初始化 - PWM 频率 50Hz (周期 20ms)
    // 72MHz / (719+1) / (1999+1) = 50Hz
    MOTOR_DUOJI2_TIM->CR1 = 0;
    MOTOR_DUOJI2_TIM->PSC = 719;
    MOTOR_DUOJI2_TIM->ARR = 1999;
    MOTOR_DUOJI2_TIM->CCR1 = 1850;              // 初始占空比 (90 度)
    MOTOR_DUOJI2_TIM->CCMR1 = TIM_OCMode_PWM2 | TIM_CCMR1_OC1PE;
    MOTOR_DUOJI2_TIM->CCER = TIM_CCER_CC1E;
    MOTOR_DUOJI2_TIM->EGR = TIM_EGR_UG;
    MOTOR_DUOJI2_TIM->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}

/****
******* 舵机 2 初始化 (保留函数，实际禁用)
*****/
void Motor_Duoji2_Init(void)
{
    // 单舵机版本不需要此函数
    // 原 TIM1 舵机接口已禁用
}

/****
******* 设置舵机比较值 (TIM3_CH1)
*****/
void Motor_Duoji_SetCompare(u16 compare)
{
    MOTOR_DUOJI2_TIM->CCR1 = compare;
}
