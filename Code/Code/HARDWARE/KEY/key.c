/**********************************
包含头文件
**********************************/
#include "key.h"
#include "delay.h"


/****
*******	KEY IO初始化
*****/
void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

 	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK_ENABLE,ENABLE);								//使能端口时钟
	GPIO_InitStructure.GPIO_Pin  = KEY1_PIN;														//设置引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 											//设置成上拉输入
 	GPIO_Init(KEY1_PORT, &GPIO_InitStructure);													//根据设定参数初始化
	GPIO_SetBits(KEY1_PORT,KEY1_PIN);						 												//IO设置输出

	RCC_APB2PeriphClockCmd(KEY2_GPIO_CLK_ENABLE,ENABLE);								//使能端口时钟
	GPIO_InitStructure.GPIO_Pin  = KEY2_PIN;														//设置引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 											//设置成上拉输入
 	GPIO_Init(KEY2_PORT, &GPIO_InitStructure);													//根据设定参数初始化
	GPIO_SetBits(KEY2_PORT,KEY2_PIN);						 												//IO设置输出

	RCC_APB2PeriphClockCmd(KEY3_GPIO_CLK_ENABLE,ENABLE);								//使能端口时钟
	GPIO_InitStructure.GPIO_Pin  = KEY3_PIN;														//设置引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 											//设置成上拉输入
 	GPIO_Init(KEY3_PORT, &GPIO_InitStructure);													//根据设定参数初始化
	GPIO_SetBits(KEY3_PORT,KEY3_PIN);						 												//IO设置输出

	RCC_APB2PeriphClockCmd(KEY4_GPIO_CLK_ENABLE,ENABLE);								//使能端口时钟
	GPIO_InitStructure.GPIO_Pin  = KEY4_PIN;														//设置引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 											//设置成上拉输入
 	GPIO_Init(KEY4_PORT, &GPIO_InitStructure);													//根据设定参数初始化
	GPIO_SetBits(KEY4_PORT,KEY4_PIN);						 												//IO设置输出

	RCC_APB2PeriphClockCmd(KEY5_GPIO_CLK_ENABLE,ENABLE);								//使能端口时钟
	GPIO_InitStructure.GPIO_Pin  = KEY5_PIN;														//设置引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 											//设置成上拉输入
 	GPIO_Init(KEY5_PORT, &GPIO_InitStructure);													//根据设定参数初始化
	GPIO_SetBits(KEY5_PORT,KEY5_PIN);

	RCC_APB2PeriphClockCmd(KEY6_GPIO_CLK_ENABLE,ENABLE);								//使能端口时钟
	GPIO_InitStructure.GPIO_Pin  = KEY6_PIN;														//设置引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 											//设置成上拉输入
 	GPIO_Init(KEY6_PORT, &GPIO_InitStructure);													//根据设定参数初始化
	GPIO_SetBits(KEY6_PORT,KEY6_PIN);

}


/****
*******	按键扫描函数 (优化版：带二次确认防抖)
优化点：
1. 延时后二次确认按键状态，提高可靠性，防止干扰误触发
2. 优先检测KEY4(报警解除)，提升紧急响应速度
3. 保持原有的单次触发机制(key_up标志位)

mode: 0-不支持连续按; 1-支持连续按
返回值: 0-无按键; 1-6对应KEY1-KEY6
*****/
u8 Chiclet_Keyboard_Scan(u8 mode)
{
	static u8 key_up=1;			//按键松开标志

	if(mode)  							//mode=1,支持连按
		key_up=1;

	if(key_up && (KEY1==0 || KEY2==0 || KEY3==0 || KEY4==0 || KEY5==0 || KEY6==0))
	{
		Delay_ms(10);					//去抖动延时

		// 二次确认：延时后再次检测，确保是真实按键而非干扰信号
		if(KEY1==0 || KEY2==0 || KEY3==0 || KEY4==0 || KEY5==0 || KEY6==0)
		{
			key_up=0;

			// 优先级调整：报警解除按键优先响应，提升紧急情况处理能力
			if(KEY4==0)						//按键4按下(报警解除)，返回4
				return 4;
			else if(KEY1==0)			//按键1按下(开关盖/触发开盖)，返回1
				return 1;
			else if(KEY2==0)			//按键2按下(UV时长+30s)，返回2
				return 2;
			else if(KEY3==0)			//按键3按下(UV时长-30s)，返回3
				return 3;
			else if(KEY5==0)			//按键5按下(手动UV启停)，返回5
				return 5;
			else if(KEY6==0)			//按键6按下(模式切换)，返回6
				return 6;
		}
	}

	else if(KEY1==1 && KEY2==1 && KEY3==1 && KEY4==1 && KEY5==1 && KEY6==1)		//没有按键按下，松开标志位为1
		key_up=1;

	return 0;							// 无按键按下，返回0
}
