#ifndef __GPIO_H
#define __GPIO_H	 
#include "sys.h"
/************************************************************
 
************************************************************/ 
#define LED0 PEout(10)		// PE10
#define LED1 PEout(11)		// PE11	

/*************IO�˿ڹ���************************************/
#define PWMD0 PDin(0)      
#define PWMD1 PDin(1)
#define PWMD2 PDin(2)

#define PWMU0 PDout(0)
#define PWMU1 PDout(3)
#define PWMU2 PDout(2)

/*****************OP�������******************************/
#define OP_D1  PDout(5)
#define OP_D2  PDout(6)



void LED_Init(void);   		  //LED ��ʼ������
void TMC_CLK_OUT(void);      //ʱ�����
void  GPIO_PWMD_PWMU(void);
u16 Get_Adc_Average(u8 ch,u8 times);
void  Adc_Init(void);

u8 GetStallState(u8 Motor);  //ʧ�ټ��
		 				    
#endif
