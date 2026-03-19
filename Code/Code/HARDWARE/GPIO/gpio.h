#ifndef __GPIO_H
#define __GPIO_H


/**********************************
 * 头文件
 **********************************/
#include "sys.h"


/**********************************
 * 引脚定义 (修改后 - 单垃圾桶版本)
 **********************************/

// LED 指示灯 (保留 1 组，用于垃圾桶状态指示)
#define LED1_G_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOA      // LED1_G 时钟
#define LED1_G_PORT                         GPIOA
#define LED1_G_PIN                          GPIO_Pin_15
#define LED1_G                              PAout(15)

#define LED1_R_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB      // LED1_R 时钟
#define LED1_R_PORT                         GPIOB
#define LED1_R_PIN                          GPIO_Pin_3
#define LED1_R                              PBout(3)

// LED2: 消毒指示 (LED2_G/PB4) + 故障指示 (LED2_R/PB5)
#define LED2_G_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB
#define LED2_G_PORT                         GPIOB
#define LED2_G_PIN                          GPIO_Pin_4
#define LED2_G                              PBout(4)

#define LED2_R_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB
#define LED2_R_PORT                         GPIOB
#define LED2_R_PIN                          GPIO_Pin_5
#define LED2_R                              PBout(5)

#define LED3_G_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB
#define LED3_G_PORT                         GPIOB
#define LED3_G_PIN                          GPIO_Pin_6
#define LED3_G                              PBout(6)

#define LED3_R_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB
#define LED3_R_PORT                         GPIOB
#define LED3_R_PIN                          GPIO_Pin_7
#define LED3_R                              PBout(7)

#define LED4_G_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB
#define LED4_G_PORT                         GPIOB
#define LED4_G_PIN                          GPIO_Pin_8
#define LED4_G                              PBout(8)

#define LED4_R_GPIO_CLK_ENABLE              RCC_APB2Periph_GPIOB
#define LED4_R_PORT                         GPIOB
#define LED4_R_PIN                          GPIO_Pin_9
#define LED4_R                              PBout(9)


// 红外传感器 (只保留 JS1 用于满溢检测)
#define IR_1_GPIO_CLK_ENABLE                RCC_APB2Periph_GPIOA      // IR_1 时钟 (JS1 - PA4)
#define IR_1_PORT                           GPIOA
#define IR_1_PIN                            GPIO_Pin_4
#define IR_1                                PAin(4)

// 新增模块 (单垃圾桶版本)
// JS2 (PA5) - 超声波测距 ECHO 输入 (HC-SR04)
// PA7       - 超声波测距 TRIG 输出 (HC-SR04，飞线连接，STM32F103C8T6 第 17 脚)
// 引脚完整定义和初始化见 HARDWARE/HCSR04/hcsr04.h 及 hcsr04.c

// SG90_1 (PA0) - 蜂鸣器输出（与舵机接口互换后的临时测试映射）
#define BEEP_GPIO_CLK_ENABLE                RCC_APB2Periph_GPIOA
#define BEEP_PORT                           GPIOA
#define BEEP_PIN                            GPIO_Pin_0
#define BEEP                                PAout(0)

// PA7 - 超声波 TRIG 输出 (HC-SR04 飞线脚)；PA7 的完整初始化在 HCSR04_Init() 中完成
// 注意：PA7 即原 JS4 信号线位置，已通过飞线连接到 HC-SR04 的 TRIG 引脚


/**********************************
 * 函数声明
 **********************************/
void Gpio_Init(void);                       // GPIO 初始化


#endif
