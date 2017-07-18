
/**********************************************************************
* Copyright (c) 2002-2016 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： TMC429.c
* 描    述： TMC429相关程序，包含初始化,读写函数
*
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
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
* 函 数 名     : Reference_search
* 函数功能		: IO 输出指令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/

void TIM4_Encoder_Init(void)
{

	GPIO_InitTypeDef           GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	TIM_ICInitTypeDef          TIM_ICInitStructure;  
   NVIC_InitTypeDef           NVIC_InitStructure;   
   
/*----------------------------------------------------------------*/
	/*PA6 ch1  A,PA7 ch2 B*/ 
   
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);       //开启AFIO 时钟   
   
   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    //使能TIM3时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //使能GPIOA时钟
   
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE); //禁止进制JTAG 功能，否则PB4 无法使用。 
   
   
	GPIO_StructInit(&GPIO_InitStructure);                    //将GPIO_InitStruct中的参数按缺省值输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;         
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IN_FLOATING;    //PA6 PA7浮空输入	
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
	TIM_TimeBaseStructure.TIM_Period = 30000;     //设定计数器重装值   
	TIM_TimeBaseStructure.TIM_Prescaler = 0;        //TIM3时钟预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision =TIM_CKD_DIV1 ;      //设置时钟分割 T_dts = T_ck_int	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);              

/*-----------------------------------------------------------------*/
                    
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, 
                                          TIM_ICPolarity_Rising);//使用编码器模式3，上升沿捕获
	TIM_ICStructInit(&TIM_ICInitStructure);      //将结构体中的内容缺省输入
	TIM_ICInitStructure.TIM_ICFilter = 6;        //选择输入比较滤波器
	TIM_ICInit(TIM3, &TIM_ICInitStructure);      //将TIM_ICInitStructure中的指定参数初始化TIM3
  
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);       //清除TIM3的更新标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //开启TIM3更新标志使能
	
   //Reset counter
	TIM3->CNT = 0;//

	TIM_Cmd(TIM3, ENABLE);   //启动TIM3定时器      
      
}

void TIM3_IRQHandler(void)
{ 		
   u16 DIR;     
   
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)//溢出中断
	{
    DIR=(TIM3->CR1)>>4&0X0001;
    if(DIR==0) 
    {  DIR_num+=1; }   

    else DIR_num-=1;    
         
	}				   
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位    
}

int Get_Encoder(void)
{
   Encoder_TIM= TIM_GetCounter(TIM3);   //获取寄存器的值
   
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











