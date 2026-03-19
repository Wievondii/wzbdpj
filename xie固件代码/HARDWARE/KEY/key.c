#include "key.h"
#include "delay.h"

/************************************************************
 * 功能: 按键初始化
 * 说明: 配置所有按键为上拉输入
 ************************************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOA和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // 禁用JTAG，释放PA15作为GPIO使用
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /*==========================================================
     * GPIOB 按键配置 (SW2-SW5)
     *==========================================================*/
    GPIO_InitStructure.GPIO_Pin = KEY_SW2_PIN | KEY_SW3_PIN | KEY_SW4_PIN | KEY_SW5_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*==========================================================
     * GPIOA 按键配置 (SW6-SW8)
     *==========================================================*/
    GPIO_InitStructure.GPIO_Pin = KEY_SW6_PIN | KEY_SW7_PIN | KEY_SW8_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/************************************************************
 * 功能: 按键扫描
 * 参数: mode - 0:不支持连按  1:支持连按
 * 返回: 按键值 (0-无按键, 2-8:对应SW2-SW8)
 ************************************************************/
uint8_t KEY_Scan(uint8_t mode)
{
    static uint8_t key_up = 1;  // 按键松开标志

    if(mode == 1)
        key_up = 1;  // 支持连按

    // 检测是否有按键按下
    if(key_up && (KEY_SW2 == 0 || KEY_SW3 == 0 || KEY_SW4 == 0 || KEY_SW5 == 0 ||
                  KEY_SW6 == 0 || KEY_SW7 == 0 || KEY_SW8 == 0))
    {
        Delay_ms(10);  // 消抖延时
        key_up = 0;

        // 判断具体哪个按键按下
        if(KEY_SW2 == 0)      return KEY_MODE_SWITCH;
        else if(KEY_SW3 == 0) return KEY_ENTRANCE_GATE;
        else if(KEY_SW4 == 0) return KEY_EXIT_GATE;
        else if(KEY_SW5 == 0) return KEY_EMERGENCY;
        else if(KEY_SW6 == 0) return KEY_MENU;
        else if(KEY_SW7 == 0) return KEY_SPACE_ADD;
        else if(KEY_SW8 == 0) return KEY_SPACE_SUB;
    }
    else if(KEY_SW2 == 1 && KEY_SW3 == 1 && KEY_SW4 == 1 && KEY_SW5 == 1 &&
            KEY_SW6 == 1 && KEY_SW7 == 1 && KEY_SW8 == 1)
    {
        key_up = 1;  // 所有按键松开
    }

    return KEY_NONE;
}
