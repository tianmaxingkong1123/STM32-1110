/**********************************************************************
* Copyright (c) 2002-2016 ɽ�����������ҵ���޹�˾ All rights reserved.
*
* �ļ����ƣ� AT25128B.c
* ��    ���� EEPROM ��д����
*
*
* �޸ļ�¼��
*
* V1.0��2006.12.22���Ǳ�
**********************************************************************/
#include "AT25128B.h"
#include "SPI.h"
#include "Globals.h"
#include "gpio.h"



/*******************************************************************************
* �� �� ��     : WriteEepromByte
* ��������		: д���ݵ�EEPROM
* ��    ��     : ��ַ
* ��    ��     : 0 ����1 ����
*******************************************************************************/
u8 WriteEepromByte(u32 Address, u8 Value)
{
   u8 i=0;
  //Schreiben erlauben
  ReadWriteSPI(SPI_DEV_EEPROM, 0x06, TRUE);                           //дʹ�ܣ�д���ر�
  do
  {
   // i++;if(i>=100)   return 0;                                      //��ֹ������ѭ��
    ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                        //��״̬   
     
  } while((ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x02)==0x00);   //�ж��Ƿ����дʹ��  
  
  ReadWriteSPI(SPI_DEV_EEPROM, 0x02, FALSE);                          //д����
  ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);                  //д��ַ
  ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);                
  ReadWriteSPI(SPI_DEV_EEPROM, Value, TRUE);                          //д����
  
  i=0;
  do
  {
    i++;if(i>=250)   return 0;                                         //д����ʱ�䣬̫�̵Ļ����
    ReadWriteSPI(SPI_DEV_EEPROM, 0x05, FALSE);                         //��״̬
  } while(ReadWriteSPI(SPI_DEV_EEPROM, 0x00, TRUE) & 0x01);            //�ж��Ƿ�д��
  
  ReadWriteSPI(SPI_DEV_EEPROM, 0x04, TRUE);
  return  1;
  
}

/*******************************************************************************
* �� �� ��     : ReadEepromByte
* ��������		: д���ݵ�EEPROM
* ��    ��     : ��ַ
* ��    ��     : 0 ����1 ����
*******************************************************************************/

u8 ReadEepromByte(u32 Address)
{
  ReadWriteSPI(SPI_DEV_EEPROM, 0x03, FALSE);                           //������
  ReadWriteSPI(SPI_DEV_EEPROM, Address >> 8, FALSE);                   //��ַ        
  ReadWriteSPI(SPI_DEV_EEPROM, Address & 0xff, FALSE);

  return ReadWriteSPI(SPI_DEV_EEPROM, 0, TRUE);                        //  ����
}

/*******************************************************************************
* �� �� ��     : ReadEepromBlock  ����
* ��������		: д���ݵ�EEPROM
* ��    ��     : ��ַ
* ��    ��     : 0 ����1 ����
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
* �� �� ��     : WriteEepromBlock  д��
* ��������		: д���ݵ�EEPROM
* ��    ��     : ��ַ
* ��    ��     : 0 ����1 ����
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
    if((Address & 0x0000003f)==0 && i<Size-1)                               //Adressbits �bergelaufen, aber noch Bytes zu schreiben?
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

      //Neuer "Write"-Befehl (mit der n�chsten Adresse)
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



