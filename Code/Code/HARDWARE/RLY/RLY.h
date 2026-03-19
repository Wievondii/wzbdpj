#ifndef __RLY_H
#define __RLY_H

#include "sys.h"
#include "stm32f10x.h"

#define RLY_GPIO_CLK_ENABLE      RCC_APB2Periph_GPIOB
#define RLY_PORT                 GPIOB
#define RLY_PIN                  GPIO_Pin_0
#define RLY 										 PBout(0)

void RLY_Init(void);
void RLY_On(void);
void RLY_Off(void);

#endif
