#ifndef  __SPI_H
#define  __SPI_H
#include "sys.h"



/********************SPI��ز���**********************/
//SPI ��ģ��ʹ������
#define SPI_TMC429CS  PDout(9)    
#define SPI_TMC262CS  PDout(8)
#define SPI_EEPROMCS  PBout(12)

#define SPI_DEV_TMC429   0
#define SPI_DEV_TMC262   1  //������չ262����
#define SPI_DEV_EEPROM   2



void SPI_TMC_EEPROM_Init(void);
void SPI2_SetSpeed(u8 DeviceNumber,u8 SPI_BaudRatePrescaler);
u8 ReadWriteSPI(u8 DeviceNumber, u8 TxData, u8 LastTransfer);


#endif



