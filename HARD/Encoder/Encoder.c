
/**********************************************************************
* Copyright (c) 2002-2016 ɽ�����������ҵ���޹�˾ All rights reserved.
*
* �ļ����ƣ� TMC429.c
* ��    ���� TMC429��س��򣬰�����ʼ��,��д����
*
*
* �޸ļ�¼��
*
* V1.0��2006.12.22���Ǳ�
**********************************************************************/
#include "delay.h"
#include "sys.h"
#include "Encoder.h"
#include "gpio.h"
#include "sys.h"
#include "usart.h"
#include "AT25128B.h"


int     Encoder_TIM=0;
int     Encoder_num=0;
u8      DIR_num=128;
u16     Encoder_pre=128;
/*******************************************************************************
* �� �� ��     : Reference_search
* ��������		: IO ���ָ��
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void TIM4_Encoder_Init(void)
{

	GPIO_InitTypeDef           GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	TIM_ICInitTypeDef          TIM_ICInitStructure;  
   NVIC_InitTypeDef           NVIC_InitStructure;   
   
/*----------------------------------------------------------------*/
	/*PA6 ch1  A,PA7 ch2 B*/ 
   
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);       //����AFIO ʱ��   
   
   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //ʹ��TIM3ʱ��	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //ʹ��GPIOAʱ��
   
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //��ֹ����JTAG ���ܣ�����PB4 �޷�ʹ�á� 
   
   
	GPIO_StructInit(&GPIO_InitStructure);                    //��GPIO_InitStruct�еĲ�����ȱʡֵ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;    //PA6 PA7��������	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);      
   
   GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);
  

/*----------------------------------------------------------------*/ 
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 
	
/*---------------------------------------------------------------------*/
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = 30000;     //�趨��������װֵ   
	TIM_TimeBaseStructure.TIM_Prescaler = 0;        //TIM3ʱ��Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;      //����ʱ�ӷָ� T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ��� 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              

/*-----------------------------------------------------------------*/
                    
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, 
                                          TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3�������ز���
	TIM_ICStructInit(&TIM_ICInitStructure);      //���ṹ���е�����ȱʡ����
	TIM_ICInitStructure.TIM_ICFilter = 6;        //ѡ������Ƚ��˲���
	TIM_ICInit(TIM3, &TIM_ICInitStructure);      //��TIM_ICInitStructure�е�ָ��������ʼ��TIM3
  
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);       //���TIM3�ĸ��±�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //����TIM3���±�־ʹ��
	
   //Reset counter
	TIM3->CNT = 0;//

	TIM_Cmd(TIM3, ENABLE);   //����TIM3��ʱ��      
      
}

void TIM3_IRQHandler(void)
{ 		
   u16 DIR;     
   
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)//����ж�
	{
    DIR=(TIM3->CR1)>>4&0X0001;
    if(DIR==0) 
    {  DIR_num+=1; }   

    else DIR_num-=1;    
         
	}				   
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ    
}

int Get_Encoder(void)
{
   Encoder_TIM= TIM_GetCounter(TIM3);   //��ȡ�Ĵ�����ֵ
   
   if(DIR_num==128)           
     {Encoder_num= Encoder_TIM;}
   else if(DIR_num==127)
     {Encoder_num= 0xffffffff-30000+Encoder_TIM;}
      
       else
       {
       if(DIR_num>128)
       Encoder_num=Encoder_TIM+30000*(DIR_num-128);
       else
       if(DIR_num<128)        
           
        
           Encoder_num= 0xffffffff-(-Encoder_TIM+30000+30000*(127-DIR_num));     
       }       
        
    if(Encoder_num>0) 
         printf("+%d",Encoder_num);
    else                
         printf("-%d ",0-Encoder_num);       
       
    return Encoder_num;   
   
}

void Set_Encoder_num(int num) 
{  if(num>0)
   {
   DIR_num=num/30000+128;
   TIM3->CNT =num%30000;
   }
   else
   {
     DIR_num=127-((-num)/30000);
     TIM3->CNT =30000-(-num)%30000;
 
   }
}











