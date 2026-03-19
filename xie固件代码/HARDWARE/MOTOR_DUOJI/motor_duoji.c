#include "motor_duoji.h"
#include "delay.h"

/************************************************************
 * 功能: 舵机初始化 (TIM2_CH3和TIM2_CH4)
 * 说明: 配置PWM输出，频率50Hz，周期20ms
 ************************************************************/
void Servo_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能GPIO和TIM2时钟
    RCC_APB2PeriphClockCmd(SERVO_ENTRANCE_GPIO_CLK, ENABLE);
    RCC_APB1PeriphClockCmd(SERVO_ENTRANCE_TIM_CLK, ENABLE);

    /*==========================================================
     * 配置GPIO为复用推挽输出
     *==========================================================*/
    // PA2 - TIM2_CH3 (入口道闸)
    GPIO_InitStructure.GPIO_Pin = SERVO_ENTRANCE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SERVO_ENTRANCE_PORT, &GPIO_InitStructure);

    // PA3 - TIM2_CH4 (出口道闸)
    GPIO_InitStructure.GPIO_Pin = SERVO_EXIT_PIN;
    GPIO_Init(SERVO_EXIT_PORT, &GPIO_InitStructure);

    /*==========================================================
     * TIM2 定时器配置
     * 时钟频率: 72MHz
     * 预分频: 72-1 (分频后1MHz)
     * 自动重装载: 20000-1 (周期20ms, 频率50Hz)
     *==========================================================*/
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;          // 自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;          // 预分频系数
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(SERVO_ENTRANCE_TIM, &TIM_TimeBaseStructure);

    /*==========================================================
     * PWM模式配置 - 通道3 (入口道闸)
     *==========================================================*/
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_Pulse = SERVO_ANGLE_0;  // 初始值：关闭状态
    TIM_OC3Init(SERVO_ENTRANCE_TIM, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(SERVO_ENTRANCE_TIM, TIM_OCPreload_Enable);

    /*==========================================================
     * PWM模式配置 - 通道4 (出口道闸)
     *==========================================================*/
    TIM_OCInitStructure.TIM_Pulse = SERVO_ANGLE_0;  // 初始值：关闭状态
    TIM_OC4Init(SERVO_ENTRANCE_TIM, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(SERVO_ENTRANCE_TIM, TIM_OCPreload_Enable);

    // 使能定时器
    TIM_ARRPreloadConfig(SERVO_ENTRANCE_TIM, ENABLE);
    TIM_Cmd(SERVO_ENTRANCE_TIM, ENABLE);
}

/************************************************************
 * 功能: 设置入口道闸舵机角度
 * 参数: angle - PWM脉宽值 (500~2500)
 ************************************************************/
void Servo_Entrance_SetAngle(uint16_t angle)
{
    TIM_SetCompare3(SERVO_ENTRANCE_TIM, angle);
}

/************************************************************
 * 功能: 设置出口道闸舵机角度
 * 参数: angle - PWM脉宽值 (500~2500)
 ************************************************************/
void Servo_Exit_SetAngle(uint16_t angle)
{
    TIM_SetCompare4(SERVO_EXIT_TIM, angle);
}

/************************************************************
 * 功能: 入口道闸开启 (缓启动)
 ************************************************************/
void Servo_Entrance_Open(void)
{
    uint16_t pulse = SERVO_ANGLE_0;

    // 缓慢启动，避免电流冲击
    while(pulse < SERVO_ANGLE_90)
    {
        pulse += 10;
        if(pulse > SERVO_ANGLE_90) pulse = SERVO_ANGLE_90;
        Servo_Entrance_SetAngle(pulse);
        Delay_ms(20);
    }
    Delay_ms(500);  // 等待舵机到位
}

/************************************************************
 * 功能: 入口道闸关闭 (缓停止)
 ************************************************************/
void Servo_Entrance_Close(void)
{
    uint16_t pulse = SERVO_ANGLE_90;

    // 缓慢停止
    while(pulse > SERVO_ANGLE_0)
    {
        pulse -= 10;
        if(pulse < SERVO_ANGLE_0) pulse = SERVO_ANGLE_0;
        Servo_Entrance_SetAngle(pulse);
        Delay_ms(20);
    }
    Delay_ms(500);  // 等待舵机到位
}

/************************************************************
 * 功能: 出口道闸开启 (缓启动)
 ************************************************************/
void Servo_Exit_Open(void)
{
    uint16_t pulse = SERVO_ANGLE_0;

    // 缓慢启动
    while(pulse < SERVO_ANGLE_90)
    {
        pulse += 10;
        if(pulse > SERVO_ANGLE_90) pulse = SERVO_ANGLE_90;
        Servo_Exit_SetAngle(pulse);
        Delay_ms(20);
    }
    Delay_ms(500);  // 等待舵机到位
}

/************************************************************
 * 功能: 出口道闸关闭 (缓停止)
 ************************************************************/
void Servo_Exit_Close(void)
{
    uint16_t pulse = SERVO_ANGLE_90;

    // 缓慢停止
    while(pulse > SERVO_ANGLE_0)
    {
        pulse -= 10;
        if(pulse < SERVO_ANGLE_0) pulse = SERVO_ANGLE_0;
        Servo_Exit_SetAngle(pulse);
        Delay_ms(20);
    }
    Delay_ms(500);  // 等待舵机到位
}
