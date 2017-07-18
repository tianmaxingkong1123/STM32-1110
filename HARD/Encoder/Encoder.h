#ifndef  __ENCODER_H
#define  __ENCODER_H

#include "sys.h"



//void TIM3_Mode_Config(void);
//extern void RCC_Configuration(void);

void TIM4_Encoder_Init(void);
int Get_Encoder(void);
void Set_Encoder_num(int num) ;

#endif

