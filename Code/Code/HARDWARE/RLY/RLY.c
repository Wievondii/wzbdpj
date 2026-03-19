#include "RLY.h"

void RLY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RLY_GPIO_CLK_ENABLE,ENABLE);								//ʹ�ܶ˿�ʱ��
	GPIO_InitStructure.GPIO_Pin  = RLY_PIN;														//��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 											//���ó���������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(RLY_PORT, &GPIO_InitStructure);													//�����趨������ʼ��
	GPIO_ResetBits(RLY_PORT,RLY_PIN);		
}

void RLY_On(void)
{
	
	GPIO_SetBits(RLY_PORT,RLY_PIN);	
	
}


void RLY_Off(void)
{
	
	GPIO_ResetBits(RLY_PORT,RLY_PIN);	
	
}

