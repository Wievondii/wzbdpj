
/**********************************
����ͷ�ļ�
**********************************/

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>

/**********************************
��������
**********************************/
uint8_t usart1_buf[64];
uint16_t usart1_cnt = 0, usart1_cntPre = 0;
uint8_t usart2_buf[64];
uint16_t usart2_cnt = 0, usart2_cntPre = 0;

uint8_t Bluetooth_RxData;		//���崮�ڽ��յ����ݱ���
uint8_t Bluetooth_RxFlag;		//���崮�ڽ��յı�־λ����


/*
************************************************************
*	�������ƣ�	Usart1_Init
*
*	�������ܣ�	����1��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA9		RX-PA10
************************************************************
*/
void Usart1_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	//NVIC_InitTypeDef nvicInitStruct;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA10	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART1, &usartInitStruct);
	
	USART_Cmd(USART1, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
		/*�ж��������*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);		
	
	/*NVIC�жϷ���*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);			
	
	
//	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
//	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
//	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;//0
//	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;//2
//	NVIC_Init(&nvicInitStruct);
	
		/*NVIC����*/
//	NVIC_InitTypeDef NVIC_InitStructure;					
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
	NVIC_Init(&NVIC_InitStructure);		
	
}

/*
************************************************************
*	�������ƣ�	Usart2_Init
*
*	�������ܣ�	����2��ʼ��
*
*	��ڲ�����	baud���趨�Ĳ�����
*
*	���ز�����	��
*
*	˵����		TX-PA2		RX-PA3
************************************************************
*/
void Usart2_Init(unsigned int baud)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	//PA2	TXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	//PA3	RXD
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_3;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	
	usartInitStruct.USART_BaudRate = baud;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//��Ӳ������
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//���պͷ���
	usartInitStruct.USART_Parity = USART_Parity_No;									//��У��
	usartInitStruct.USART_StopBits = USART_StopBits_1;								//1λֹͣλ
	usartInitStruct.USART_WordLength = USART_WordLength_8b;							//8λ����λ
	USART_Init(USART2, &usartInitStruct);
	
	USART_Cmd(USART2, ENABLE);														//ʹ�ܴ���
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);									//ʹ�ܽ����ж�
	
	nvicInitStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvicInitStruct);
	
	
}

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		USART_SendData(USARTx, *str++);									//��������
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}

}

/*
************************************************************
*	�������ƣ�	UsartPrintf
*
*	�������ܣ�	��ʽ����ӡ
*
*	��ڲ�����	USARTx��������
*				fmt����������
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
{
	/* stubbed out to save flash (vsnprintf costs ~3KB) */
	(void)USARTx; (void)fmt;
}

/*
************************************************************
*	�������ƣ�	USART1_Clear
//
*	�������ܣ�	��ջ���
//
*	��ڲ�����	��
//
*	���ز�����	��
//
*	˵����		
************************************************************
*/
void USART1_Clear(void)
{
	memset(usart1_buf, 0, sizeof(usart1_buf));
	usart1_cnt = 0;
}

/*
************************************************************
*	�������ƣ�	USART1_WaitRecive
//
*	�������ܣ�	�ȴ��������
//
*	��ڲ�����	��
//
*	���ز�����	0-�������		1-���ճ�ʱδ���
//
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool USART1_WaitRecive(void)
{
	if(usart1_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 1;
		
	if(usart1_cnt == usart1_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		usart1_cnt = 0;							//��0���ռ���
			
		return 0;								//���ؽ�����ɱ�־
	}
		
	usart1_cntPre = usart1_cnt;					//��Ϊ��ͬ
	
	return 1;								//���ؽ���δ��ɱ�־
}

/*
************************************************************
*	�������ƣ�	USART1_Clear
//
*	�������ܣ�	��ջ���
//
*	��ڲ�����	��
//
*	���ز�����	��
//
*	˵����		
************************************************************
*/
void USART2_Clear(void)
{
	memset(usart2_buf, 0, sizeof(usart2_buf));
	usart2_cnt = 0;
}

/*
************************************************************
*	�������ƣ�	USART2_WaitRecive
//
*	�������ܣ�	�ȴ��������
//
*	��ڲ�����	��
//
*	���ز�����	0-�������		1-���ճ�ʱδ���
//
*	˵����		ѭ�����ü���Ƿ�������
************************************************************
*/
_Bool USART2_WaitRecive(void)
{

	if(usart2_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return 1;
		
	if(usart2_cnt == usart2_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		usart2_cnt = 0;							//��0���ռ���
			
		return 0;								//���ؽ�����ɱ�־
	}
		
	usart2_cntPre = usart2_cnt;					//��Ϊ��ͬ
	
	return 1;								//���ؽ���δ��ɱ�־
}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����2�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/

//void USART1_IRQHandler(void)
//{
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //�����ж�
//	{
//		if(usart1_cnt >= sizeof(usart1_buf))	usart1_cnt = 0; //��ֹ���ڱ�ˢ��
//		usart1_buf[usart1_cnt++] = USART1->DR;
//		
//		USART_ClearFlag(USART1, USART_FLAG_RXNE);
//	}
//}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		
	{
		Bluetooth_RxData = USART_ReceiveData(USART1);				
		Bluetooth_RxFlag = 1;										
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);		
	}
}

/*
************************************************************
*	�������ƣ�	USART1_IRQHandler
*
*	�������ܣ�	����2�շ��ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(usart2_cnt >= sizeof(usart2_buf))	usart2_cnt = 0; //��ֹ���ڱ�ˢ��
		usart2_buf[usart2_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}


uint8_t Bluetooth_GetRxFlag(void)
{
	if (Bluetooth_RxFlag == 1)			
	{
		Bluetooth_RxFlag = 0;
		return 1;					
	}
	return 0;						
}


uint8_t Bluetooth_GetRxData(void)
{
	return Bluetooth_RxData;			
}

