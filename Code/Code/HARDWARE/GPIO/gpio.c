/**********************************
 * 头文件
 **********************************/
#include "gpio.h"


/****
******* LED IO 初始化
*****/
void Gpio_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 关闭 JTAG，启用 SWD


    // LED1_G (PA15) - 垃圾桶状态绿灯
    RCC_APB2PeriphClockCmd(LED1_G_GPIO_CLK_ENABLE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED1_G_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_G_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED1_G_PORT, LED1_G_PIN);

    // LED1_R (PB3) - 垃圾桶状态红灯
    RCC_APB2PeriphClockCmd(LED1_R_GPIO_CLK_ENABLE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED1_R_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED1_R_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED1_R_PORT, LED1_R_PIN);

    // LED2_G (PB4) - 消毒工作指示灯
    RCC_APB2PeriphClockCmd(LED2_G_GPIO_CLK_ENABLE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED2_G_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED2_G_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED2_G_PORT, LED2_G_PIN);    // 初始关闭

    // LED2_R (PB5) - 故障报警指示灯
    RCC_APB2PeriphClockCmd(LED2_R_GPIO_CLK_ENABLE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LED2_R_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED2_R_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED2_R_PORT, LED2_R_PIN);    // 初始关闭

    // LED3_G (PB6) - 报警流水灯
    GPIO_InitStructure.GPIO_Pin = LED3_G_PIN;
    GPIO_Init(LED3_G_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED3_G_PORT, LED3_G_PIN);

    // LED3_R (PB7) - 报警流水灯
    GPIO_InitStructure.GPIO_Pin = LED3_R_PIN;
    GPIO_Init(LED3_R_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED3_R_PORT, LED3_R_PIN);

    // LED4_G (PB8) - 报警流水灯
    GPIO_InitStructure.GPIO_Pin = LED4_G_PIN;
    GPIO_Init(LED4_G_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED4_G_PORT, LED4_G_PIN);

    // LED4_R (PB9) - 报警流水灯
    GPIO_InitStructure.GPIO_Pin = LED4_R_PIN;
    GPIO_Init(LED4_R_PORT, &GPIO_InitStructure);
    GPIO_SetBits(LED4_R_PORT, LED4_R_PIN);

    // IR_1 (PA4) - 红外满溢检测 (JS1)
    RCC_APB2PeriphClockCmd(IR_1_GPIO_CLK_ENABLE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = IR_1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入：传感器触发时拉低
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_1_PORT, &GPIO_InitStructure);

    // 注意：
    // - PA5 (JS2) 已改为超声波测距 ECHO 输入，在 HCSR04_Init() 中配置为浮空输入
    // - PA7 (飞线) 已改为超声波测距 TRIG 输出，在 HCSR04_Init() 中配置为推挽输出
    // - PA6 (JS3) 已改为舵机 PWM 输出，在 Motor_Duoji_Init() 中配置
    // - PA0 (SG90_1) 已改为蜂鸣器输出，在 Beep_Init() 中配置
} 
