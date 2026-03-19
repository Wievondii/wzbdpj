#include "gpio.h"

/************************************************************
 * 功能: GPIO 初始化配置
 * 说明: 配置红外传感器输入、蜂鸣器输出、LED输出
 ************************************************************/
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能时钟和AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // 禁用JTAG，释放PB3/PB4引脚作为GPIO使用
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /*==========================================================
     * 车位检测红外传感器配置 (输入上拉)
     *==========================================================*/
    // IR_SPACE1 (PB2)
    GPIO_InitStructure.GPIO_Pin = IR_SPACE1_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IR_SPACE1_PORT, &GPIO_InitStructure);

    // IR_SPACE2 (PB4)
    GPIO_InitStructure.GPIO_Pin = IR_SPACE2_PIN;
    GPIO_Init(IR_SPACE2_PORT, &GPIO_InitStructure);

    // IR_SPACE3 (PB6)
    GPIO_InitStructure.GPIO_Pin = IR_SPACE3_PIN;
    GPIO_Init(IR_SPACE3_PORT, &GPIO_InitStructure);

    /*==========================================================
     * 入口/出口车辆检测红外传感器配置 (输入上拉)
     *==========================================================*/
    // IR_ENTRANCE (PA0)
    GPIO_InitStructure.GPIO_Pin = IR_ENTRANCE_PIN;
    GPIO_Init(IR_ENTRANCE_PORT, &GPIO_InitStructure);

    // IR_EXIT (PA1)
    GPIO_InitStructure.GPIO_Pin = IR_EXIT_PIN;
    GPIO_Init(IR_EXIT_PORT, &GPIO_InitStructure);

    /*==========================================================
     * 蜂鸣器控制输出 (推挽输出)
     *==========================================================*/
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);  // 初始关闭蜂鸣器

    /*==========================================================
     * LED 报警灯输出 (推挽输出)
     *==========================================================*/
    GPIO_InitStructure.GPIO_Pin = LED_ALARM_PIN;
    GPIO_Init(LED_ALARM_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(LED_ALARM_PORT, LED_ALARM_PIN);  // 初始关闭LED
}
