/**********************************************************************
* Copyright (c) 2002-2016 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： SPI.c
* 描    述： SPI相关程序，包含初始化,读写函数
*
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
* 测试正常 2016.12.28
**********************************************************************/

#include "SPI.h"
#include "delay.h"

/*******************************************************************************
* 函 数 名     : TMC_EEPROM_SPI_Init
* 函数功能		: 初始化SPI0，SPI1
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/

void SPI_TMC_EEPROM_Init(void)   
{
  /*********************SPI2 初始化*****************************/
   
   GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;   
   
   /**************片选引脚初始化,262使能引脚**************************/
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;           //PD8,9
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //推挽输出
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //速度
   GPIO_Init(GPIOD,&GPIO_InitStructure);                        //初始化 IO
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_6;          //PD8,9
   GPIO_Init(GPIOB,&GPIO_InitStructure);                        //初始化 IO   
   
   
   SPI_TMC429CS=1;   
   SPI_TMC262CS=1;
   SPI_EEPROMCS=1;
 //  ENABLE_DRIVERS=0;                             //262使能引脚   
   
   
  /*******************SPI2初始化****************************/ 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);        //GPIO口时钟时钟
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);         //SPI时钟使能 

   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;   //PB13,14,15
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                      //复用推挽输出
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                    //速度
   GPIO_Init(GPIOB,&GPIO_InitStructure);                       //初始化 IO
         
   GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);    //引脚上拉
   
  
   SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;   //设置SPI 模式，双向全双工
   SPI_InitStructure.SPI_Mode=SPI_Mode_Master;                 //模式，主机模式
   SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;             //数据长度选择
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;                   //串行同步时钟的空闲状态为高电平
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;                  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
   SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                     //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
   SPI_InitStructure.SPI_BaudRatePrescaler=
                                 SPI_BaudRatePrescaler_4;      //定义波特率预分频的值:波特率预分频值为32   36/32
   SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;            //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
   SPI_InitStructure.SPI_CRCPolynomial=7;	                     //CRC值计算的多项式
   
   SPI_Init (SPI2,&SPI_InitStructure);                         //初始化PSI
   
   SPI_Cmd (SPI2,ENABLE);                                      //使能SPI 外设
   
/********************SPI1 初始化*****************************/

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);       //引脚上拉
   
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;                    //串行同步时钟的空闲状态为高电平
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;                   //串行同步时钟的第二个跳变沿（上升或下降）数据被采�
   SPI_InitStructure.SPI_BaudRatePrescaler=
                                    SPI_BaudRatePrescaler_64;   //SPI 波特率,
                                  
   SPI_Init(SPI1,&SPI_InitStructure);   
   SPI_Cmd(SPI1,ENABLE);   

}
/*******************************************************************************
* 函 数 名     : SPI2_SetSpeed
* 函数功能		: SPI 速度设置函数 
* 输    入     : 分频值，器件 SPI_BaudRatePrescaler_8   2,4,8,16,32,64,128,256
* 输    出     : 无
*******************************************************************************/  
void SPI2_SetSpeed(u8 DeviceNumber,u8 SPI_BaudRatePrescaler)
{ 
   
   assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
   
   switch(DeviceNumber)
   {
      case   SPI_DEV_TMC429:
      case   SPI_DEV_TMC262:               
               SPI1->CR1&=0XFFC7;
               SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
               SPI_Cmd(SPI1,ENABLE); 
          break;
      case   SPI_DEV_EEPROM:
               SPI2->CR1&=0XFFC7;
               SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度 
               SPI_Cmd(SPI2,ENABLE); 
          break;
      default :
         break;
   }
} 

/*******************************************************************************
* 函 数 名     : ReadWriteSPI
* 函数功能		: SPI 读写函数
* 输    入     : SPI器件，数据，是否继续读
* 输    出     : 读取到的数据
*******************************************************************************/
u8 ReadWriteSPI(u8 DeviceNumber, u8 TxData, u8 LastTransfer)
{
  u8 Result; 
  u8 retry=0;
  switch(DeviceNumber)           //SPI 器件进行片选
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
               while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
                  {
                  retry++;
                  if(retry>200) return 0;
                  }			  
               SPI_I2S_SendData(SPI1, TxData);     //通过外设SPIx发送一个数据
               retry=0;

               while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
                  {
                  retry++;
                  if(retry>200) return 0;
                  }	  						    
               Result=SPI_I2S_ReceiveData(SPI1);   //返回通过SPIx最近接收的数据	
       break;

    case SPI_DEV_EEPROM:    
               //delay_ms(20);
               while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
                  {
                  retry++;
                  if(retry>200)return 0;
                  }			  
               SPI_I2S_SendData(SPI2, TxData); //通过外设SPIx发送一个数据
               retry=0;

               while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
                  {
                  retry++;
                  if(retry>200)return 0;
                  }	  						    
               Result=SPI_I2S_ReceiveData(SPI2); //返回通过SPIx最近接收的数据	
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







