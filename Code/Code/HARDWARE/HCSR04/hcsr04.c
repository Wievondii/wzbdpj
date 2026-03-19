/**********************************
 * HC-SR04 超声波测距模块驱动
 *
 * 工作原理：
 *   1. MCU 向 TRIG 发送 ≥10 μs 的高电平脉冲
 *   2. HC-SR04 自动发射 8 次 40 kHz 超声波
 *   3. ECHO 引脚在发射期间拉高
 *   4. 收到回波后 ECHO 拉低
 *   5. 距离 (cm) = ECHO 高电平时间 (μs) × 0.017
 *
 * 引脚连接（详见 hcsr04.h）：
 *   TRIG → PA7 (飞线到 STM32 芯片第 17 脚)
 *   ECHO → PA5 (JS2 接口信号脚，PCB 已有焊盘)
 *   VCC  → JS2 接口 +5V
 *   GND  → JS2 接口 GND
 **********************************/

#include "hcsr04.h"
#include "delay.h"

/**
 * @brief  HC-SR04 引脚初始化
 *         TRIG (PA7): 推挽输出，初始低电平
 *         ECHO (PA5): 浮空输入
 */
void HCSR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 开启 GPIOA 时钟 */
    RCC_APB2PeriphClockCmd(HCSR04_TRIG_GPIO_CLK_ENABLE, ENABLE);

    /* PA7 - TRIG：推挽输出，初始低电平 */
    GPIO_InitStructure.GPIO_Pin   = HCSR04_TRIG_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_TRIG_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);  /* TRIG 默认低电平 */

    /* PA5 - ECHO：浮空输入（HC-SR04 主动驱动该线） */
    GPIO_InitStructure.GPIO_Pin   = HCSR04_ECHO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(HCSR04_ECHO_PORT, &GPIO_InitStructure);
}

/**
 * @brief  获取超声波测量距离
 * @return 距离，单位 cm；超出量程或超时返回 999
 *
 * 注意：
 *   - HC-SR04 有效量程：2 ~ 400 cm
 *   - 当前所用宽电压多模式模块在 GPIO 模式下建议测量间隔 ≥ 200 ms
 *   - 测量期间不应有大幅度主循环延时抢占（约需 30 ms）
 */
uint16_t HCSR04_GetDistance(void)
{
    uint32_t elapsed_us = 0;
    uint16_t distance;
    static uint16_t last_distance = 100;  /* 上次有效距离，用于简单滤波 */

    /* 1. 发送 TRIG 触发脉冲（15 μs > 最小要求 10 μs） */
    GPIO_SetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);
    Delay_us(15);
    GPIO_ResetBits(HCSR04_TRIG_PORT, HCSR04_TRIG_PIN);

    /* 2. 等待 ECHO 变高（HC-SR04 启动发射，最长约 1 ms） */
    elapsed_us = 0;
    while (GPIO_ReadInputDataBit(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == 0)
    {
        Delay_us(1);
        elapsed_us++;
        if (elapsed_us > 5000u)         /* 5 ms 超时：模块未响应 */
            return 999u;
    }

    /* 3. 计算 ECHO 高电平持续时间（每 1 μs 累加） */
    elapsed_us = 0;
    while (GPIO_ReadInputDataBit(HCSR04_ECHO_PORT, HCSR04_ECHO_PIN) == 1)
    {
        Delay_us(1);
        elapsed_us++;
        if (elapsed_us > HCSR04_TIMEOUT_US)   /* 超过最大量程 */
        {
            last_distance = 400u;
            return 999u;
        }
    }

    /* 4. 计算距离：distance(cm) = elapsed_us × 0.017
     *    整数运算：distance = elapsed_us × 17 / 1000           */
    distance = (uint16_t)(elapsed_us * 17u / 1000u);

    /* 限制有效范围 */
    if (distance < 2u)   distance = 2u;
    if (distance > 400u) distance = 400u;

    /* 简单均值滤波，平滑偶发抖动 */
    distance = (uint16_t)((distance + last_distance) / 2u);
    last_distance = distance;

    return distance;
}
