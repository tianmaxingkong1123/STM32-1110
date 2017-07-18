/**********************************************************************
* Copyright (c) 2002-2016 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： AT25128B.c
* 描    述： EEPROM 读写程序
*
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
**********************************************************************/
#include "AT25128B.h"
#include "SPI.h"
#include "Globals.h"
#include "gpio.h"



/*******************************************************************************
* 函 数 名     : WriteEepromByte
* 函数功能		: 写数据到EEPROM
* 输    入     : 地址
* 输    出     : 0 错误，1 正常
*******************************************************************************/
u8 WriteEepromByte(u32 Address, u8 Value)
{
   u8 i=0;
  //Schreiben erlauben
  ReadWriteSPI(SPI_DEV_EEPROM, 0x06, TRUE);                           //写使能，写完后关闭
  do
  {
   // i++;if(i>=100)   return 0;                                      //防止出错，死循环
    ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                        //读状态   
     
  } while((ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x02)==0x00);   //判断是否打开了写使能  
  
  ReadWriteSPI(SPI_DEV_EEPROM, 0x02, FALSE);                          //写命令
  ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);                  //写地址
  ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);                
  ReadWriteSPI(SPI_DEV_EEPROM, Value, TRUE);                          //写数据
  
  i=0;
  do
  {
    i++;if(i>=250)   return 0;                                         //写数据时间，太短的会出错
    ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                         //读状态
  } while(ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x01);            //判断是否写完
  
  ReadWriteSPI(SPI_DEV_EEPROM, 0x04, TRUE);
  return  1;
  
}

/*******************************************************************************
* 函 数 名     : ReadEepromByte
* 函数功能		: 写数据到EEPROM
* 输    入     : 地址
* 输    出     : 0 错误，1 正常
*******************************************************************************/

u8 ReadEepromByte(u32 Address)
{
  ReadWriteSPI(SPI_DEV_EEPROM, 0x03, FALSE);                           //读命令
  ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);                   //地址        
  ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);

  return ReadWriteSPI(SPI_DEV_EEPROM, 0, TRUE);                        //  数据
}

/*******************************************************************************
* 函 数 名     : ReadEepromBlock  读块
* 函数功能		: 写数据到EEPROM
* 输    入     : 地址
* 输    出     : 0 错误，1 正常
*******************************************************************************/
void ReadEepromBlock(u32 Address, u8 *Block, u32 Size)
{
  u32 i;

  ReadWriteSPI(SPI_DEV_EEPROM, 0x03, FALSE);                        //Befehl "Read"
  ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);
  ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);

  for(i=0; i<Size; i++)
    *(Block+i)=ReadWriteSPI(SPI_DEV_EEPROM, 0, i==Size-1);         //beim letzten Byte EEPROM deselektieren

}


/*******************************************************************************
* 函 数 名     : WriteEepromBlock  写块
* 函数功能		: 写数据到EEPROM
* 输    入     : 地址
* 输    出     : 0 错误，1 正常
*******************************************************************************/
u8 WriteEepromBlock(u32 Address, u8 *Block, u32 Size)
{
  u32 i;

  //Schreiben erlauben
  ReadWriteSPI(SPI_DEV_EEPROM, 0x06, TRUE);                           //Befehl "Write Enable"
  do
  {
    // i++;if(i>=200)  LED1=0; //return 0;     
     ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                       //Befehl "Get Status"
  } while((ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x02)==0x00);   //Warte bis "Write Enable"-Bit gesetzt

  //Schreibvorgang (Startadresse)
  ReadWriteSPI(SPI_DEV_EEPROM, 0x02, FALSE); //Befehl "Write"
  ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);
  ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);

  //Eigentliches Schreiben der Daten
  for(i=0; i<Size; i++)
  {
    
    Address++;
    ReadWriteSPI(SPI_DEV_EEPROM, *(Block+i),  (Address & 0x0000003f)==0 || i==Size-1);
    if((Address & 0x0000003f)==0 && i<Size-1)                               //Adressbits 黚ergelaufen, aber noch Bytes zu schreiben?
    {
      //Warte bis Schreibvorgang beendet
      do
      {
        ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                          //Befehl "Get Status"
      } while(ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x01);

      //Neuer "Write Enable"-Befehl
      ReadWriteSPI(SPI_DEV_EEPROM, 0x06, TRUE);                            //Befehl "Write Enable"
      do
      {
        ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                            //Befehl "Get Status"
      } while((ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x02)==0x00);    //Warte bis "Write Enable"-Bit gesetzt

      //Neuer "Write"-Befehl (mit der n鋍hsten Adresse)
      ReadWriteSPI(SPI_DEV_EEPROM, 0x02, FALSE); //Befehl "Write"
      ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);
      ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);
    }
  }

  //Warte bis Schreibvorgang beendet
  do
  {
    ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                  //Befehl "Get Status"
  } while(ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x01);
  
  return  1;
}



