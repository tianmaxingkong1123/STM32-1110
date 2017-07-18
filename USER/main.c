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
* 1110板程序
* 时间：2016.12.21 
* 技术支持：  
* 作者：      乔彬
* 开发进度： 
* 外部晶振16M  ,HSE=16M, PLLSRC=HSE/2,  PLLCLK=79,  倍频系数9 
* 需要修改  "stm32f10x_it.h"中的外部时钟频率数值
* 需要修改  时钟配置的时钟来源和分频
************************************************/

 u32 Tick_Counter=0;

 int main(void)
 {	
    u32 now_Tick_Counter=0;
    // u8 i=0;
    
	delay_init();	                                    //延时函数初始化	  
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //设置NVIC中断分组2:2位抢占优
   uart_init(115200);	 	                           //串口初始化为115200
    
   usmart_dev.init(SystemCoreClock/1000000);	         //初始化USMART	
    
	LED_Init();		  	                                 //初始化与LED连接的硬件接口
   GPIO_PWMD_PWMU(); 
   Adc_Init();
   TIM4_Encoder_Init();
   //CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,brp,CAN_Mode_Normal);//CAN初始化环回模式,波特率500Kbps
   //CAN_Init_ID()
    
   TMC_CLK_OUT() ;
   SPI_TMC_EEPROM_Init(); 
   CAN_Init_ID(); 
    
   Init429();   
   InitMotorDrivers();    
    
 //  printf("\r\n您发送的消息为:1234567899\r\n\r\n");  //测试串口通讯        
    
     // WriteEepromByte(0x0000,60);      
      if(ReadEepromByte(0x0000)==60)   LED1=0;         //测试EEPROM
      else  LED1=1;
   
  while(1)
    {		
       usmart_dev.scan();                              //调试助手 
         
       PromcessCommands() ;                            //指令处理
       SystemControl();                                //系统控制函数     
         
       if((Tick_Counter-now_Tick_Counter)>=500)         
       {         
       
           LED0=~LED0;
           now_Tick_Counter=Tick_Counter;
           if(Tick_Counter>1000000) Tick_Counter=0;  //防止溢出 
         
       }       
	 }   
}



 

	


