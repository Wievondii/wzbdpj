#ifndef __KEY_H
#define __KEY_H


/**********************************
包含头文件
**********************************/
#include "sys.h" 
 

/**********************************
重定义关键词
**********************************/
#define KEY1_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOB		//按键1引脚
#define KEY1_PORT                 GPIOB
#define KEY1_PIN                  GPIO_Pin_15
#define KEY1  										PBin(15)

#define KEY2_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOB		//按键2引脚
#define KEY2_PORT                 GPIOB
#define KEY2_PIN                  GPIO_Pin_14
#define KEY2  										PBin(14)

#define KEY3_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOB		//按键3引脚
#define KEY3_PORT                 GPIOB
#define KEY3_PIN                  GPIO_Pin_13
#define KEY3  										PBin(13)

#define KEY4_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOB		//按键4引脚
#define KEY4_PORT                 GPIOB
#define KEY4_PIN                  GPIO_Pin_12
#define KEY4  										PBin(12)


#define KEY5_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOB		//按键5引脚
#define KEY5_PORT                 GPIOB
#define KEY5_PIN                  GPIO_Pin_1
#define KEY5  										PBin(1)

#define KEY6_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOA		//按键5引脚
#define KEY6_PORT                 GPIOA
#define KEY6_PIN                  GPIO_Pin_12
#define KEY6  										PAin(12)


/**********************************
函数声明
**********************************/
void Key_Init(void);																		//IO初始化
u8 Chiclet_Keyboard_Scan(u8 mode);  										//按键扫描函数


#endif

