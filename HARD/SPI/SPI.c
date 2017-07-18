/**********************************************************************
* Copyright (c) 2002-2016 ɽ�����������ҵ���޹�˾ All rights reserved.
*
* �ļ����ƣ� SPI.c
* ��    ���� SPI��س��򣬰�����ʼ��,��д����
*
*
* �޸ļ�¼��
*
* V1.0��2006.12.22���Ǳ�
* �������� 2016.12.28
**********************************************************************/

#include "SPI.h"
#include "delay.h"

/*******************************************************************************
* �� �� ��     : TMC_EEPROM_SPI_Init
* ��������		: ��ʼ��SPI0��SPI1
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void SPI_TMC_EEPROM_Init(void)   
{
  /*********************SPI2 ��ʼ��*****************************/
   
   GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;   
   
   /**************Ƭѡ���ų�ʼ��,262ʹ������**************************/
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOD,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;           //PD8,9
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;               //�������
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;              //�ٶ�
   GPIO_Init(GPIOD,&GPIO_InitStructure);                        //��ʼ�� IO
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_6;          //PD8,9
   GPIO_Init(GPIOB,&GPIO_InitStructure);                        //��ʼ�� IO   
   
   
   SPI_TMC429CS=1;   
   SPI_TMC262CS=1;
   SPI_EEPROMCS=1;
 //  ENABLE_DRIVERS=0;                             //262ʹ������   
   
   
  /*******************SPI2��ʼ��****************************/ 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);        //GPIO��ʱ��ʱ��
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);         //SPIʱ��ʹ�� 

   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;   //PB13,14,15
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;                      //�����������
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;                    //�ٶ�
   GPIO_Init(GPIOB,&GPIO_InitStructure);                       //��ʼ�� IO
         
   GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);    //��������
   
  
   SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;   //����SPI ģʽ��˫��ȫ˫��
   SPI_InitStructure.SPI_Mode=SPI_Mode_Master;                 //ģʽ������ģʽ
   SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;             //���ݳ���ѡ��
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;                   //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;                  //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
   SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;                     //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
   SPI_InitStructure.SPI_BaudRatePrescaler=
                                 SPI_BaudRatePrescaler_4;      //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ32   36/32
   SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;            //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
   SPI_InitStructure.SPI_CRCPolynomial=7;	                     //CRCֵ����Ķ���ʽ
   
   SPI_Init (SPI2,&SPI_InitStructure);                         //��ʼ��PSI
   
   SPI_Cmd (SPI2,ENABLE);                                      //ʹ��SPI ����
   
/********************SPI1 ��ʼ��*****************************/

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7; 
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);       //��������
   
   SPI_InitStructure.SPI_CPOL=SPI_CPOL_High;                    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
   SPI_InitStructure.SPI_CPHA=SPI_CPHA_2Edge;                   //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ����
   SPI_InitStructure.SPI_BaudRatePrescaler=
                                    SPI_BaudRatePrescaler_64;   //SPI ������,
                                  
   SPI_Init(SPI1,&SPI_InitStructure);   
   SPI_Cmd(SPI1,ENABLE);   

}
/*******************************************************************************
* �� �� ��     : SPI2_SetSpeed
* ��������		: SPI �ٶ����ú��� 
* ��    ��     : ��Ƶֵ������ SPI_BaudRatePrescaler_8   2,4,8,16,32,64,128,256
* ��    ��     : ��
*******************************************************************************/  
void SPI2_SetSpeed(u8 DeviceNumber,u8 SPI_BaudRatePrescaler)
{ 
   
   assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
   
   switch(DeviceNumber)
   {
      case   SPI_DEV_TMC429:
      case   SPI_DEV_TMC262:               
               SPI1->CR1&=0XFFC7;
               SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
               SPI_Cmd(SPI1,ENABLE); 
          break;
      case   SPI_DEV_EEPROM:
               SPI2->CR1&=0XFFC7;
               SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
               SPI_Cmd(SPI2,ENABLE); 
          break;
      default :
         break;
   }
} 

/*******************************************************************************
* �� �� ��     : ReadWriteSPI
* ��������		: SPI ��д����
* ��    ��     : SPI���������ݣ��Ƿ������
* ��    ��     : ��ȡ��������
*******************************************************************************/
u8 ReadWriteSPI(u8 DeviceNumber, u8 TxData, u8 LastTransfer)
{
  u8 Result; 
  u8 retry=0;
  switch(DeviceNumber)           //SPI ��������Ƭѡ
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
               while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
                  {
                  retry++;
                  if(retry>200) return 0;
                  }			  
               SPI_I2S_SendData(SPI1, TxData);     //ͨ������SPIx����һ������
               retry=0;

               while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
                  {
                  retry++;
                  if(retry>200) return 0;
                  }	  						    
               Result=SPI_I2S_ReceiveData(SPI1);   //����ͨ��SPIx������յ�����	
       break;

    case SPI_DEV_EEPROM:    
               //delay_ms(20);
               while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
                  {
                  retry++;
                  if(retry>200)return 0;
                  }			  
               SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
               retry=0;

               while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
                  {
                  retry++;
                  if(retry>200)return 0;
                  }	  						    
               Result=SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	
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







