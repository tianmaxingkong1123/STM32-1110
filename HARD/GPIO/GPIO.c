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

* 本文件包含各IO口初始化函数
* 本文件定义了各端口的输入输出 
* 本文件随时更新
* 本文件配制了程序中所有的
**********************************************************************/

#include "GPIO.h"
#include "delay.h"


/*******************************************************************************
* 函 数 名     : LED_Init
* 函数功能		: LED端口初始化
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/

void LED_Init(void)
{	 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|
												RCC_APB2Periph_GPIOB, ENABLE);	       		//使能PE端口时钟
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;				            //LED0-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					         //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 					         //IO口速度为50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);					 								//根据设定参数初始化GPIOE.10,11
	 GPIO_SetBits(GPIOE,GPIO_Pin_10|GPIO_Pin_11);						 				   //PB.10 输出高
  
   /*******************初始化失速检测引脚**************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				                        //LED5-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 					            //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 					         //IO口速度为50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);					 								//根据设定参数初始化GPIOE.10,11
	 GPIO_ResetBits(GPIOE,GPIO_Pin_9);						 				               //PB.10 输出高
	 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					         //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 					         //IO口速度为50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 								//根据设定参数初始化GPIOE.10,11
	 GPIO_ResetBits(GPIOB,GPIO_Pin_6);						 				               //PB.10 输出高
        
}

/*******************************************************************************
* 函 数 名     : TMC_CLK_OUT
* 函数功能		: TMC芯片时钟供给
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/

void TMC_CLK_OUT(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);        //使能GPIOA 时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;               //复用推挽输出
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;             //输出频率
   
   GPIO_Init(GPIOA,&GPIO_InitStructure);   
   
   RCC_MCOConfig(RCC_MCO_HSE);                                 //选择输出的时钟来源，外部时钟源  16M

}

/*******************************************************************************
* 函 数 名     : GPIO_PWMU_GPIO_PWMUD
* 函数功能		: GPIO输出，或输入
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void  GPIO_PWMD_PWMU(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD ,ENABLE);              //初始化GPIOC 引脚
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;      //引脚端口
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                 
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;                        //下拉输入
   
   GPIO_Init(GPIOC,&GPIO_InitStructure);                              //初始化
   GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);            //
   
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;                     //推挽输出
   GPIO_Init(GPIOD,&GPIO_InitStructure);                              //初始化端口 D
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;                 //引脚端口
   GPIO_Init(GPIOD,&GPIO_InitStructure);    
   
   //GPIO_SetBits(GPIOD,GPIO_Pin_6|GPIO_Pin_5);                       //拉高
   GPIO_ResetBits(GPIOD,GPIO_Pin_6|GPIO_Pin_5);
   GPIO_ResetBits(GPIOD,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);            //拉高
   
 }  
 
/*******************************************************************************
* 函 数 名     : GPIO_INOUT
* 函数功能		: 设置相应的IO口为输入输出功能
* 输    入     : 方向，引脚，IO
* 输    出     : 无
*******************************************************************************/
void GPIO_INOUT(u8 INOUT,u8 Pin,u8 UD)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   if(INOUT)
      {
      GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
      }
   else 
     { 
      GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
     }
     if(Pin==0)
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
     else if(Pin==1)
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
     else
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
     
     if(UD==1)      GPIO_Init(GPIOD,&GPIO_InitStructure);
         else       GPIO_Init(GPIOC,&GPIO_InitStructure);

}
   

/*******************************************************************************
* 函 数 名     : GetStallState
* 函数功能		: 获取262 失速引脚状态
* 输    入     : 目标262 
* 输    出     : 状态 1 失速 0 正常
*******************************************************************************/

u8 GetStallState(u8 Motor)
{
   if(Motor==0)
   {
    return PEin(8);
   }
   /**********扩展多个262*********/
   else return  0x0;
}


		   
/*******************************************************************************
* 函 数 名     : Adc_Init
* 函数功能		: 初始化ADC
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/															   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	    //ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	       //模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	 //模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	               //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	                     //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	                                 //使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	                           //使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	            //等待复位校准结束
	
	ADC_StartCalibration(ADC1);	                           //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	                  //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}	


/*******************************************************************************
* 函 数 名     : Get_Adc
* 函数功能		: 获得ADC值
* 输    入     : ch:通道值 0~3
* 输    出     : 值
*******************************************************************************/		
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		      //使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));    //等待转换结束

	return ADC_GetConversionValue(ADC1);	            //返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 
 
 
