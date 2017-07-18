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

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "GPIO.h"
#include "usmart.h"	
#include "SPI.h"
#include "TMC429.h"
#include "TMC262.h"
#include "AT25128B.h"
#include "Control.h"
#include "can.h"
#include "Commands.h"
#include "Encoder.h"
 
/************************************************
* 1110�����
* ʱ�䣺2016.12.21 
* ����֧�֣�  
* ���ߣ�      �Ǳ�
* �������ȣ� 
* �ⲿ����16M  ,HSE=16M, PLLSRC=HSE/2,  PLLCLK=79,  ��Ƶϵ��9 
* ��Ҫ�޸�  "stm32f10x_it.h"�е��ⲿʱ��Ƶ����ֵ
* ��Ҫ�޸�  ʱ�����õ�ʱ����Դ�ͷ�Ƶ
************************************************/

 u32 Tick_Counter=0;

 int main(void)
 {	
    u32 now_Tick_Counter=0;
    // u8 i=0;
    
	delay_init();	                                    //��ʱ������ʼ��	  
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //����NVIC�жϷ���2:2λ��ռ��
   uart_init(115200);	 	                           //���ڳ�ʼ��Ϊ115200
    
   usmart_dev.init(SystemCoreClock/1000000);	         //��ʼ��USMART	
    
	LED_Init();		  	                                 //��ʼ����LED���ӵ�Ӳ���ӿ�
   GPIO_PWMD_PWMU(); 
   Adc_Init();
   TIM4_Encoder_Init();
   //CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,brp,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������500Kbps
   //CAN_Init_ID()
    
   TMC_CLK_OUT() ;
   SPI_TMC_EEPROM_Init(); 
   CAN_Init_ID(); 
    
   Init429();   
   InitMotorDrivers();    
    
 //  printf("\r\n�����͵���ϢΪ:1234567899\r\n\r\n");  //���Դ���ͨѶ        
    
     // WriteEepromByte(0x0000,60);      
      if(ReadEepromByte(0x0000)==60)   LED1=0;         //����EEPROM
      else  LED1=1;
   
  while(1)
    {		
       usmart_dev.scan();                              //�������� 
         
       PromcessCommands() ;                            //ָ���
       SystemControl();                                //ϵͳ���ƺ���     
         
       if((Tick_Counter-now_Tick_Counter)>=500)         
       {         
       
           LED0=~LED0;
           now_Tick_Counter=Tick_Counter;
           if(Tick_Counter>1000000) Tick_Counter=0;  //��ֹ��� 
         
       }       
	 }   
}



 

	


