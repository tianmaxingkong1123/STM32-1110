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

* ���ļ�������IO�ڳ�ʼ������
* ���ļ������˸��˿ڵ�������� 
* ���ļ���ʱ����
* ���ļ������˳��������е�
**********************************************************************/

#include "GPIO.h"
#include "delay.h"


/*******************************************************************************
* �� �� ��     : LED_Init
* ��������		: LED�˿ڳ�ʼ��
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void LED_Init(void)
{	 
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|
												RCC_APB2Periph_GPIOB, ENABLE);	       		//ʹ��PE�˿�ʱ��
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;				            //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					         //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 					         //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);					 								//�����趨������ʼ��GPIOE.10,11
	 GPIO_SetBits(GPIOE,GPIO_Pin_10|GPIO_Pin_11);						 				   //PB.10 �����
  
   /*******************��ʼ��ʧ�ټ������**************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				                        //LED5-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 					            //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 					         //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);					 								//�����趨������ʼ��GPIOE.10,11
	 GPIO_ResetBits(GPIOE,GPIO_Pin_9);						 				               //PB.10 �����
	 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					         //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 					         //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);					 								//�����趨������ʼ��GPIOE.10,11
	 GPIO_ResetBits(GPIOB,GPIO_Pin_6);						 				               //PB.10 �����
        
}

/*******************************************************************************
* �� �� ��     : TMC_CLK_OUT
* ��������		: TMCоƬʱ�ӹ���
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void TMC_CLK_OUT(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);        //ʹ��GPIOA ʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;               //�����������
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;             //���Ƶ��
   
   GPIO_Init(GPIOA,&GPIO_InitStructure);   
   
   RCC_MCOConfig(RCC_MCO_HSE);                                 //ѡ�������ʱ����Դ���ⲿʱ��Դ  16M

}

/*******************************************************************************
* �� �� ��     : GPIO_PWMU_GPIO_PWMUD
* ��������		: GPIO�����������
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void  GPIO_PWMD_PWMU(void)
{
   GPIO_InitTypeDef  GPIO_InitStructure;
   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD ,ENABLE);              //��ʼ��GPIOC ����
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;      //���Ŷ˿�
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                 
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;                        //��������
   
   GPIO_Init(GPIOC,&GPIO_InitStructure);                              //��ʼ��
   GPIO_ResetBits(GPIOC,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);            //
   
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;                     //�������
   GPIO_Init(GPIOD,&GPIO_InitStructure);                              //��ʼ���˿� D
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6;                 //���Ŷ˿�
   GPIO_Init(GPIOD,&GPIO_InitStructure);    
   
   //GPIO_SetBits(GPIOD,GPIO_Pin_6|GPIO_Pin_5);                       //����
   GPIO_ResetBits(GPIOD,GPIO_Pin_6|GPIO_Pin_5);
   GPIO_ResetBits(GPIOD,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2);            //����
   
 }  
 
/*******************************************************************************
* �� �� ��     : GPIO_INOUT
* ��������		: ������Ӧ��IO��Ϊ�����������
* ��    ��     : �������ţ�IO
* ��    ��     : ��
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
* �� �� ��     : GetStallState
* ��������		: ��ȡ262 ʧ������״̬
* ��    ��     : Ŀ��262 
* ��    ��     : ״̬ 1 ʧ�� 0 ����
*******************************************************************************/

u8 GetStallState(u8 Motor)
{
   if(Motor==0)
   {
    return PEin(8);
   }
   /**********��չ���262*********/
   else return  0x0;
}


		   
/*******************************************************************************
* �� �� ��     : Adc_Init
* ��������		: ��ʼ��ADC
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/															   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	    //ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	       //ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	 //ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	               //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	                     //����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	                                 //ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	                           //ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	            //�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	                           //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	                  //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}	


/*******************************************************************************
* �� �� ��     : Get_Adc
* ��������		: ���ADCֵ
* ��    ��     : ch:ͨ��ֵ 0~3
* ��    ��     : ֵ
*******************************************************************************/		
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		      //ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));    //�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	            //�������һ��ADC1�������ת�����
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
 
 
