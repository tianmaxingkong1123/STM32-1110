/**********************************************************************
* Copyright (c) 2002-2016 É½¶«²©¿ÆÉúÎï²úÒµÓÐÏÞ¹«Ë¾ All rights reserved.
*
* ÎÄ¼þÃû³Æ£º SPI.c
* Ãè    Êö£º SPIÏà¹Ø³ÌÐò£¬°üº¬³õÊ¼»¯,¶ÁÐ´º¯Êý
*
*
* ÐÞ¸Ä¼ÇÂ¼£º
*
* V1.0£¬2006.12.22£¬ÇÇ±ò
* ²âÊÔÕý³£ 2016.12.28
**********************************************************************/

#include "SPI.h"
#include "delay.h"

/*******************************************************************************
* º¯ Êý Ãû     : TMC_EEPROM_SPI_Init
* º¯Êý¹¦ÄÜ		: ³õÊ¼»¯SPI0£¬SPI1
* Êä    Èë     : ÎÞ
* Êä    ³ö     : ÎÞ
*******************************************************************************/

void SPI_TMC_EEPROM_Init(void)   
{
  /*********************SPI2 ³õÊ¼»¯*****************************/
   
   GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;   
   
   /**************Æ¬Ñ¡Òý½Å³õÊ¼»¯,262Ê¹ÄÜÒý½Å**************************/
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;           //PD8,9
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //ÍÆÍìÊä³ö
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //ËÙ¶È
   GPIO_Init(GPIOD,&GPIO_InitStructure);                        //³õÊ¼»¯ IO
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_6;          //PD8,9
   GPIO_Init(GPIOB,&GPIO_InitStructure);                        //³õÊ¼»¯ IO   
   
   
   SPI_TMC429CS=1;   
   SPI_TMC262CS=1;
   SPI_EEPROMCS=1;
 //  ENABLE_DRIVERS=0;                             //262Ê¹ÄÜÒý½Å   
   
   
  /*******************SPI2³õÊ¼»¯****************************/ 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);        //GPIO¿ÚÊ±ÖÓÊ±ÖÓ
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);         //SPIÊ±ÖÓÊ¹ÄÜ 

   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;   //PB13,14,15
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                      //¸´ÓÃÍÆÍìÊä³ö
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                    //ËÙ¶È
   GPIO_Init(GPIOB,&GPIO_InitStructure);                       //³õÊ¼»¯ IO
         
   GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);    //Òý½ÅÉÏÀ­
   
  
   SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;   //ÉèÖÃSPI Ä£Ê½£¬Ë«ÏòÈ«Ë«¹¤
   SPI_InitStructure.SPI_Mode=SPI_Mode_Master;                 //Ä£Ê½£¬Ö÷»úÄ£Ê½
   SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;             //Êý¾Ý³¤¶ÈÑ¡Ôñ
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;                   //´®ÐÐÍ¬²½Ê±ÖÓµÄ¿ÕÏÐ×´Ì¬Îª¸ßµçÆ½
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;                  //´®ÐÐÍ¬²½Ê±ÖÓµÄµÚ¶þ¸öÌø±äÑØ£¨ÉÏÉý»òÏÂ½µ£©Êý¾Ý±»²ÉÑù
   SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                     //NSSÐÅºÅÓÉÓ²¼þ£¨NSS¹Ü½Å£©»¹ÊÇÈí¼þ£¨Ê¹ÓÃSSIÎ»£©¹ÜÀí:ÄÚ²¿NSSÐÅºÅÓÐSSIÎ»¿ØÖÆ
   SPI_InitStructure.SPI_BaudRatePrescaler=
                                 SPI_BaudRatePrescaler_4;      //¶¨Òå²¨ÌØÂÊÔ¤·ÖÆµµÄÖµ:²¨ÌØÂÊÔ¤·ÖÆµÖµÎª32   36/32
   SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;            //Ö¸¶¨Êý¾Ý´«Êä´ÓMSBÎ»»¹ÊÇLSBÎ»¿ªÊ¼:Êý¾Ý´«Êä´ÓMSBÎ»¿ªÊ¼
   SPI_InitStructure.SPI_CRCPolynomial=7;	                     //CRCÖµ¼ÆËãµÄ¶àÏîÊ½
   
   SPI_Init (SPI2,&SPI_InitStructure);                         //³õÊ¼»¯PSI
   
   SPI_Cmd (SPI2,ENABLE);                                      //Ê¹ÄÜSPI ÍâÉè
   
/********************SPI1 ³õÊ¼»¯*****************************/

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);       //Òý½ÅÉÏÀ­
   
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;                    //´®ÐÐÍ¬²½Ê±ÖÓµÄ¿ÕÏÐ×´Ì¬Îª¸ßµçÆ½
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;                   //´®ÐÐÍ¬²½Ê±ÖÓµÄµÚ¶þ¸öÌø±äÑØ£¨ÉÏÉý»òÏÂ½µ£©Êý¾Ý±»²ÉÑ
   SPI_InitStructure.SPI_BaudRatePrescaler=
                                    SPI_BaudRatePrescaler_64;   //SPI ²¨ÌØÂÊ,
                                  
   SPI_Init(SPI1,&SPI_InitStructure);   
   SPI_Cmd(SPI1,ENABLE);   

}
/*******************************************************************************
* º¯ Êý Ãû     : SPI2_SetSpeed
* º¯Êý¹¦ÄÜ		: SPI ËÙ¶ÈÉèÖÃº¯Êý 
* Êä    Èë     : ·ÖÆµÖµ£¬Æ÷¼þ SPI_BaudRatePrescaler_8   2,4,8,16,32,64,128,256
* Êä    ³ö     : ÎÞ
*******************************************************************************/  
void SPI2_SetSpeed(u8 DeviceNumber,u8 SPI_BaudRatePrescaler)
{ 
   
   assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
   
   switch(DeviceNumber)
   {
      case   SPI_DEV_TMC429:
      case   SPI_DEV_TMC262:               
               SPI1->CR1&=0XFFC7;
               SPI1->CR1|=SPI_BaudRatePrescaler;	//ÉèÖÃSPI2ËÙ¶È 
               SPI_Cmd(SPI1,ENABLE); 
          break;
      case   SPI_DEV_EEPROM:
               SPI2->CR1&=0XFFC7;
               SPI2->CR1|=SPI_BaudRatePrescaler;	//ÉèÖÃSPI2ËÙ¶È 
               SPI_Cmd(SPI2,ENABLE); 
          break;
      default :
         break;
   }
} 

/*******************************************************************************
* º¯ Êý Ãû     : ReadWriteSPI
* º¯Êý¹¦ÄÜ		: SPI ¶ÁÐ´º¯Êý
* Êä    Èë     : SPIÆ÷¼þ£¬Êý¾Ý£¬ÊÇ·ñ¼ÌÐø¶Á
* Êä    ³ö     : ¶ÁÈ¡µ½µÄÊý¾Ý
*******************************************************************************/
u8 ReadWriteSPI(u8 DeviceNumber, u8 TxData, u8 LastTransfer)
{
  u8 Result; 
  u8 retry=0;
  switch(DeviceNumber)           //SPI Æ÷¼þ½øÐÐÆ¬Ñ¡
  {
    case SPI_DEV_TMC429:
        SPI_TMC429CS=0;
      break;

    case SPI_DEV_TMC262:
       SPI_TMC262CS=0;
      break;

    case SPI_DEV_EEPROM:
       SPI_EEPROMCS=0;
      break;

    default:
       // return  0;
      break;
		
  }

  switch(DeviceNumber)
  {
    case SPI_DEV_TMC429:
    case SPI_DEV_TMC262:
               while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //¼ì²éÖ¸¶¨µÄSPI±êÖ¾Î»ÉèÖÃÓë·ñ:·¢ËÍ»º´æ¿Õ±êÖ¾Î»
                  {
                  retry++;
                  if(retry>200) return 0;
                  }			  
               SPI_I2S_SendData(SPI1, TxData);     //Í¨¹ýÍâÉèSPIx·¢ËÍÒ»¸öÊý¾Ý
               retry=0;

               while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //¼ì²éÖ¸¶¨µÄSPI±êÖ¾Î»ÉèÖÃÓë·ñ:½ÓÊÜ»º´æ·Ç¿Õ±êÖ¾Î»
                  {
                  retry++;
                  if(retry>200) return 0;
                  }	  						    
               Result=SPI_I2S_ReceiveData(SPI1);   //·µ»ØÍ¨¹ýSPIx×î½ü½ÓÊÕµÄÊý¾Ý	
       break;

    case SPI_DEV_EEPROM:    
               //delay_ms(20);
               while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //¼ì²éÖ¸¶¨µÄSPI±êÖ¾Î»ÉèÖÃÓë·ñ:·¢ËÍ»º´æ¿Õ±êÖ¾Î»
                  {
                  retry++;
                  if(retry>200)return 0;
                  }			  
               SPI_I2S_SendData(SPI2, TxData); //Í¨¹ýÍâÉèSPIx·¢ËÍÒ»¸öÊý¾Ý
               retry=0;

               while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //¼ì²éÖ¸¶¨µÄSPI±êÖ¾Î»ÉèÖÃÓë·ñ:½ÓÊÜ»º´æ·Ç¿Õ±êÖ¾Î»
                  {
                  retry++;
                  if(retry>200)return 0;
                  }	  						    
               Result=SPI_I2S_ReceiveData(SPI2); //·µ»ØÍ¨¹ýSPIx×î½ü½ÓÊÕµÄÊý¾Ý	
      break;

    default:
        Result=0;
      break;
  }

  if(LastTransfer)
  {
    SPI_TMC429CS=1;
    SPI_TMC262CS=1;
    SPI_EEPROMCS=1;
  }

  return Result;
}







