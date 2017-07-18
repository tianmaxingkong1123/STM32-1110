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
#include "TMC429.h"
#include "Globals.h"
#include "SPI.h"
#include "TMC262.h"
#include "AT25128B.h"


/*******************************************************************************
* �� �� ��     : ReadWrite429
* ��������		: 429 ��д����,32λ
* ��    ��     : ��дָ��
* ��    ��     : ����ָ��
*******************************************************************************/

void ReadWrite429(u8 *Read, u8 *Write)
{
   u8 Motor;
   Motor=Write[0]&0x60>>5;
   if(Motor<NUM_Motor)
   {
     if(!(Write[0] & TMC429_READ) && (Write[0]<0x60) && !MotorDisable[Motor]               
                                  && ((Write[0] & 0x9f)==IDX_XTARGET || (Write[0] & 0x9f)==IDX_VTARGET))   //дĿ��λ�û��ٶ�
       {
         if(StandbyFlag[Motor])                                //��ס��־
         {
           SetMotorCurrent(Motor, MotorConfig[Motor].IRun);    //�����˶�����  262
           StandbyFlag[Motor]=FALSE;                           //����
         }

         if(FreewheelingActive[Motor])                         //����״̬
         {
           Enable262(WHICH_262(Motor));                        //����״̬�£��ر�MOS
           FreewheelingActive[Motor]=FALSE;
         }
       }
   }
   Read[0]=ReadWriteSPI(SPI_DEV_TMC429, Write[0], FALSE);
   Read[1]=ReadWriteSPI(SPI_DEV_TMC429, Write[1], FALSE);
   Read[2]=ReadWriteSPI(SPI_DEV_TMC429, Write[2], FALSE);
   Read[3]=ReadWriteSPI(SPI_DEV_TMC429, Write[3], TRUE);

}
/*******************************************************************************
* �� �� ��     : Write429Bytes
* ��������		: 429 ��д����  һ���ֽ�
* ��    ��     : д����
* ��    ��     : ��
*******************************************************************************/
void Write429Bytes(u8 Address, u8 *Bytes)
{
  u8 Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Bytes[0];
  Write[2]=Bytes[1];
  Write[3]=Bytes[2];
  ReadWrite429(Read, Write);
}
/*******************************************************************************
* �� �� ��     : Write429Datagram
* ��������		: 429 ��д����    д����λ ��������
* ��    ��     : ��ַ����/��/��λ
* ��    ��     : ��
*******************************************************************************/
void Write429Datagram(u8 Address, u8 HighByte, u8 MidByte, u8 LowByte)
{
  u8 Write[4], Read[4];

  Write[0]=Address;
  Write[1]=HighByte;
  Write[2]=MidByte;
  Write[3]=LowByte;
  ReadWrite429(Read, Write);
}
/*******************************************************************************
* �� �� ��     : Write429Zero
* ��������		: 429 ��д����    �Ĵ���д0
* ��    ��     : ��ַ/ָ��
* ��    ��     : ��
*******************************************************************************/
void Write429Zero(u8 Address)
{
  u8 Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=0;
  Write[3]=0;

  ReadWrite429(Read, Write);
}

/*******************************************************************************
* �� �� ��     : Write429Short
* ��������		: 429 ��д����    ��16λд��
* ��    ��     : ��ַ/ָ��
* ��    ��     : ��
*******************************************************************************/

void Write429Short(u8 Address, int Value)
{
  u8 Write[4], Read[4];

  Write[0]=Address;
  Write[1]=0;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite429(Read, Write);
}
/*******************************************************************************
* �� �� ��     : Write429Int
* ��������		: 429 ��д����    ��24λд��
* ��    ��     : ��ַ/ָ��
* ��    ��     : ��
*******************************************************************************/
void Write429Int(u8 Address, int Value)
{
  u8 Write[4], Read[4];

  Write[0]=Address;
  Write[1]=Value >> 16;
  Write[2]=Value >> 8;
  Write[3]=Value & 0xff;

  ReadWrite429(Read, Write);
}
/*******************************************************************************
* �� �� ��     : Read429Status
* ��������		: 429 ��д����    
* ��    ��     : ��
* ��    ��     : ����������
*******************************************************************************/
u8 Read429Status(void)
{
  return ReadWriteSPI(SPI_DEV_TMC429, 0x01, TRUE);
}
/*******************************************************************************
* �� �� ��     : Read429Bytes
* ��������		: 429 ��д����    
* ��    ��     : ��
* ��    ��     : ����������
*******************************************************************************/
u8 Read429Bytes(u8 Address, u8 *Bytes)
{
  u8 Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Bytes[0]=Read[1];
  Bytes[1]=Read[2];
  Bytes[2]=Read[3];

  return Read[0];
}

/*******************************************************************************
* �� �� ��     : Read429SingleByte
* ��������		: 429 ��д����    
* ��    ��     : ��ַ,Ҫ��ȡ���Լ�λ��
* ��    ��     : ����������
*******************************************************************************/
u8 Read429SingleByte(u8 Address, u8 Index)
{
  u8 Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  return Read[Index];
}
/*******************************************************************************
* �� �� ��     : Read429Short
* ��������		: 429 ��д����    
* ��    ��     : ��ַ,
* ��    ��     : �ֽ�
*******************************************************************************/
int Read429Short(u8 Address)
{
  u8 Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Result=(Read[2]<<8)|(Read[3]);
  if(Result & 0x00000800) Result|=0xfffff000;    //����

  return Result;
}
/*******************************************************************************
* �� �� ��     : Read429Int
* ��������		: 429 ��д����    
* ��    ��     : ��ַ,
* ��    ��     : �����ֽڣ�����������λ��
*******************************************************************************/
int Read429Int(u8 Address)
{
  u8 Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Result=(Read[1]<<16)|(Read[2]<<8)|(Read[3]);
  if(Result & 0x00800000) Result|=0xff000000;

  return Result;
}
/*******************************************************************************
* �� �� ��     : Set429RampMode
* ��������		: ģʽѡ��    
* ��    ��     : �ᣬģʽ
* ��    ��     : ��
*******************************************************************************/

void Set429RampMode(u8 Axis, u8 RampMode)
{
  u8 Write[4], Read[4];

  Write[0] = Axis<<5|IDX_REFCONF_RM|TMC429_READ;    //MOTOR_NUMBER(Axis)
  ReadWrite429(Read, Write); 

  Write[0] = Axis<<5|IDX_REFCONF_RM;                //MOTOR_NUMBER(Axis)
  Write[1] = Read[1];
  Write[2] = Read[2];
  Write[3] = RampMode;
  ReadWrite429(Read, Write);
}
/*******************************************************************************
* �� �� ��     : Set429SwitchMode
* ��������		: ����ģʽѡ��    
* ��    ��     : �ᣬģʽ
* ��    ��     : ��
*******************************************************************************/
void Set429SwitchMode(u8 Axis, u8 SwitchMode)
{
  u8 Write[4], Read[4];

  Write[0] = Axis<<5|IDX_REFCONF_RM|TMC429_READ;
  ReadWrite429(Read, Write);

  Write[0] = Axis<<5|IDX_REFCONF_RM;
  Write[1] = Read[1];
  Write[2] = SwitchMode;
  Write[3] = Read[3];
  ReadWrite429(Read, Write);
}

/*******************************************************************************
* �� �� ��     : SetAMax
* ��������		: ���ٶ�����   
* ��    ��     : ������ٶ�
* ��    ��     : ��
*******************************************************************************/
u8 SetAMax(u8 Motor, u32 AMax)
{
  int pdiv, pmul, pm, pd;
  float p, p_reduced;
  int ramp_div;
  int pulse_div;
  u8 PulseRampDiv;
  u8 Data[3];

  AMax&=0x000007ff;
  Read429Bytes(IDX_PULSEDIV_RAMPDIV|Motor<<5, Data);
  PulseRampDiv=Data[1];
  pulse_div=PulseRampDiv>>4;                                     //����PULSE_DIV 
  ramp_div=PulseRampDiv & 0x0f;                                  //����RAMP_DIV

  pm=-1; pd=-1;                                                  // -1 indicates : no valid pair found

  if(ramp_div>=pulse_div)
    p = AMax / ( 128.0 * (1<<(ramp_div-pulse_div)));             //Exponent positive or 0
  else
    p = AMax / ( 128.0 / (1<<(pulse_div-ramp_div)));             //Exponent negative

  p_reduced =p*0.988;                                           //ssssss

  for (pdiv=0; pdiv<=13; pdiv++)
  {
    pmul = (int)(p_reduced * 8.0 * (1<<pdiv)) - 128;

    if ( (0 <= pmul) && (pmul <= 127) )
    {
      pm = pmul + 128;
      pd = pdiv;
    }
  }

  Data[0]=0;
  Data[1]=(u8) pm;
  Data[2]=(u8) pd;
  Write429Bytes(Motor<<5|IDX_PMUL_PDIV, Data);
  Write429Short((Motor<<5)|IDX_AMAX, AMax);

  return 0;
}
/*******************************************************************************
* �� �� ��     : HardStop
* ��������		: �ٶ����� ��ֹͣ��� 
* ��    ��     : ������ٶ�
* ��    ��     : ��
*******************************************************************************/
void HardStop(u8 Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);    //����ģʽ��
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);  //��ǰ�ٶȺ�Ŀ���ٶ�Ϊ0
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VACTUAL);
}
/*******************************************************************************
* �� �� ��     : Init429
* ��������		: 429 ��ʼ��  
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Init429(void)
{
  u8 addr;
  u8 Motor;
  Set429_EEPROM();             //��ȡ�����
  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)         //��ʼ�������
      Write429Zero(addr|(Motor<<5));                //�����������Ϊ0
  }
                                      //��λ�źŷ�ת������ʹ��ʱ��Ҫȡ��
  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_SDO_INT|IFCONF_EN_REFR);//|0x01  ��λ��תָ��
  //Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_SDO_INT);   //|IFCONF_EN_REFR|IFCONF_SDO_INT);     //����SD ģʽ��ʹ���ҿ���
  Write429Datagram(IDX_SMGP, 0x00, 0x00, 0x02);                  //���õ��������ΪSPI ģʽ��

  //��������ٶ�����ٶȲ�����
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00,             //��Ƶ���ã�Ӱ������ٶȣ�������Ƶ�ʣ� ���ٶȲ�����spi ģʽ�µķ�Ƶ                  
                     (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x00); 
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, 
                     (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x00);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, 
                     (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x00);
  
  //������λ���ص�ģʽ���˶�RAMP ģʽ��ֹͣģʽ���ο�ģʽ��������λ���أ��˴�û����������λ
  
  
  
  Write429Datagram(IDX_REFCONF_RM|MOTOR0, 0x00, 
                   STOP_REF|(MotorConfig[0].right_limit<<1)| MotorConfig[0].left_limit, RM_RAMP);   //��λ����ģʽѡ�� 
  Write429Datagram(IDX_REFCONF_RM|MOTOR1, 0x00, 
                   STOP_REF|(MotorConfig[1].right_limit<<1)| MotorConfig[0].left_limit, RM_RAMP);   //��λ����ģʽѡ�� 
  Write429Datagram(IDX_REFCONF_RM|MOTOR2, 0x00, 
                   STOP_REF|(MotorConfig[2].right_limit<<1)| MotorConfig[2].left_limit, RM_RAMP);   //��λ����ģʽѡ�� 

  Write429Short(IDX_VMIN|MOTOR0, MotorConfig[0].mini_speed);                              //������С�ٶ� 1
  Write429Short(IDX_VMIN|MOTOR1, MotorConfig[1].mini_speed);
  Write429Short(IDX_VMIN|MOTOR2, MotorConfig[2].mini_speed);

  Write429Int(IDX_VMAX|MOTOR0, MotorConfig[0].VMax);              //�����ٶ�
  SetAMax(0, MotorConfig[0].AMax);                                //���ü��ٶ�
  Write429Int(IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
  SetAMax(1, MotorConfig[1].AMax);
  Write429Int(IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
  SetAMax(2, MotorConfig[2].AMax);
}
/*******************************************************************************
* �� �� ��     : Read429VERSION
* ��������		: ��ȡ429�汾��Ϣ 
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
u32 Read429VERSION(void)   
{
  u8 Read[4], Write[4];
  u32 Result;  Write[0]=IDX_TYPE_VERSION_429|TMC429_READ;
  ReadWrite429(Read, Write);
   
  Result=(Read[1]<<16)|(Read[2]<<8)|(Read[3]);
  if(Result & 0x00800000) Result|=0xff000000;
   
  return Result;
}   

void Set429_EEPROM(void)
{
   
  if(ReadEepromByte(0x10)!=55)
  {
  //�����洢��EEPROM
  WriteEepromBlock(0X100,(u8*)&MotorConfig[0],36);
  WriteEepromBlock(0X130,(u8*)&MotorConfig[1],36); 
  WriteEepromBlock(0X160,(u8*)&MotorConfig[2],36);
  WriteEepromByte(0x10,55);
     
  WriteEepromBlock(0X300,(u8*)&MotorConfig[0],36);  
  }
   
  ReadEepromBlock(0x100,(u8*)&MotorConfig[0],36);
  ReadEepromBlock(0x130,(u8*)&MotorConfig[1],36);
  ReadEepromBlock(0x160,(u8*)&MotorConfig[2],36);
  
 // ReadEepromBlock(0x400,(u8*)&MotorConfig2[0],12)
  
           
}



