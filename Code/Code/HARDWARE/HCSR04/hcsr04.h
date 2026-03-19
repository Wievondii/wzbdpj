#ifndef __HCSR04_H__
#define __HCSR04_H__

/**********************************
 * 头文件
 **********************************/
#include "sys.h"

/**********************************
 * HC-SR04 超声波测距模块引脚定义
 *
 * 硬件连接说明：
 *   TRIG (触发)  → PA7  【飞线】STM32F103C8T6 芯片第 17 脚
 *                         PCB 上 JS2 接口只有一根信号线 (PA5/ECHO)，
 *                         TRIG 需额外飞线到 STM32 芯片 PA7 腿上。
 *   ECHO (回声)  → PA5  【原 JS2 接口信号脚，PCB 已有焊盘】
 *   VCC          → JS2 接口的 +5V 脚
 *   GND          → JS2 接口的 GND 脚
 *
 * STM32F103C8T6 (LQFP-48) 引脚位置：
 *   PA5 = 第 15 脚 (ECHO, JS2 信号脚)
 *   PA6 = 第 16 脚 (蜂鸣器 JS3)
 *   PA7 = 第 17 脚 (TRIG, 飞线目标)
 **********************************/

/* TRIG 触发引脚 (PA7) - 飞线连接 */
#define HCSR04_TRIG_GPIO_CLK_ENABLE     RCC_APB2Periph_GPIOA
#define HCSR04_TRIG_PORT                GPIOA
#define HCSR04_TRIG_PIN                 GPIO_Pin_7
#define HCSR04_TRIG                     PAout(7)

/* ECHO 回声引脚 (PA5) - JS2 接口信号脚，PCB 已有焊盘 */
#define HCSR04_ECHO_GPIO_CLK_ENABLE     RCC_APB2Periph_GPIOA
#define HCSR04_ECHO_PORT                GPIOA
#define HCSR04_ECHO_PIN                 GPIO_Pin_5
#define HCSR04_ECHO                     PAin(5)

/* 测距超时：30 000 μs ≈ 510 cm，超出后返回 999 表示无目标 */
#define HCSR04_TIMEOUT_US               30000u

/**********************************
 * 函数声明
 **********************************/
void     HCSR04_Init(void);             // 初始化 TRIG/ECHO 引脚
uint16_t HCSR04_GetDistance(void);      // 获取距离 (单位: cm)，无目标返回 999

#endif /* __HCSR04_H__ */
