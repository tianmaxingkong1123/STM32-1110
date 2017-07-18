/**********************************************************************
* Copyright (c) 2002-2016 ɽ�����������ҵ���޹�˾ All rights reserved.
*
* �ļ����ƣ� TMC429.c
* ��    ���� TMC429��س��򣬰�����ʼ��,��д����
*
* �޸ļ�¼��
*
* V1.0��2006.12.22���Ǳ�
**********************************************************************/
#include "TMC262.h"
#include "SPI.h"
#include "Globals.h"
#include "at25128b.h"


/*******************************************************************************
* �� �� ��     : ReadWrite262
* ��������		: 429 ��д����,32λ
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void ReadWrite262(u8 Which262, u32 *ReadInt, u32 WriteInt)
{
   if(Which262==0)
   {
     *ReadInt=ReadWriteSPI(SPI_DEV_TMC262, WriteInt>>16, FALSE);
     *ReadInt<<=8;
     *ReadInt|=ReadWriteSPI(SPI_DEV_TMC262, WriteInt>>8, FALSE);
     *ReadInt<<=8;
     *ReadInt|=ReadWriteSPI(SPI_DEV_TMC262, WriteInt & 0xff, TRUE);
     *ReadInt>>=4;
   }
   else if(Which262==1)
   {       
     //�������262      
   }
   else
    {    
     //������չ262
    }
}

/*******************************************************************************
* �� �� ��     : WriteStepDirConfig
* ��������		: 262  ģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/

void WriteStepDirConfig(u8 Which262)
{
  SPIWriteInt=0;
  if(StepDirConfig[Which262].Intpol)   SPIWriteInt|=0x00000200;     
  if(StepDirConfig[Which262].DEdge)    SPIWriteInt|=0x00000100;
  if(StepDirConfig[Which262].MRes>15)  StepDirConfig[Which262].MRes=15;    //ϸ�֣����
 
  SPIWriteInt|=StepDirConfig[Which262].MRes;                               //��ַ0000

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);                        //д262�Ĵ���
}
/*******************************************************************************
* �� �� ��     : WriteStepDirConfig
* ��������		: ն��ģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void WriteChopperConfig(u8 Which262)
{
  if(ChopperConfig[Which262].BlankTime>3)       ChopperConfig[Which262].BlankTime=3;
  if(ChopperConfig[Which262].HysteresisDecay>3) ChopperConfig[Which262].HysteresisDecay=3;
  if(ChopperConfig[Which262].HysteresisEnd>15)  ChopperConfig[Which262].HysteresisEnd=15;
  if(ChopperConfig[Which262].HysteresisStart>7) ChopperConfig[Which262].HysteresisStart=7;
  if(ChopperConfig[Which262].TOff>15)           ChopperConfig[Which262].TOff=15;

  SPIWriteInt=0;
  SPIWriteInt|=0x00080000;                                           //1 CHOPCONF;
  SPIWriteInt|=((u32) ChopperConfig[Which262].BlankTime) << 15;
  if(ChopperConfig[Which262].ChopperMode) SPIWriteInt|=0x00004000;    //mode λ   14
  if(ChopperConfig[Which262].RandomTOff) SPIWriteInt|=0x00002000;     //ram  λ   13
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisDecay) << 11;
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisEnd) << 7;
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisStart) << 4;
   
  if(!ChopperConfig[Which262].DisableFlag)                      //���ر�
      SPIWriteInt|=((u32)  ChopperConfig[Which262].TOff);       //wenn DisableFlag gesetzt wird 0 gesendet

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}

/*******************************************************************************
* �� �� ��     : WriteSmartEnergyControl
* ��������		: ����ģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void WriteSmartEnergyControl(u8 Which262)
{
  if(SmartEnergyControl[Which262].SmartIMin>1)           SmartEnergyControl[Which262].SmartIMin=1;
  if(SmartEnergyControl[Which262].SmartDownStep>3)       SmartEnergyControl[Which262].SmartDownStep=3;
  if(SmartEnergyControl[Which262].SmartStallLevelMax>15) SmartEnergyControl[Which262].SmartStallLevelMax=15;
  if(SmartEnergyControl[Which262].SmartUpStep>3)         SmartEnergyControl[Which262].SmartUpStep=3;
  if(SmartEnergyControl[Which262].SmartStallLevelMin>15) SmartEnergyControl[Which262].SmartStallLevelMin=15;

  SPIWriteInt=0;
  SPIWriteInt|=0X000A0000;                                            //Registeradresse SMARTEN
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartIMin) << 15;         //��Сֵ
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartDownStep)  << 13;    //˥���ٶ�
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartStallLevelMax) << 8; //
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartUpStep) << 5;        //������С
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartStallLevelMin);       

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* �� �� ��     : WriteSmartEnergyControl
* ��������		: ����ģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void WriteStallGuardConfig(u8 Which262)
{
  if(StallGuardConfig[Which262].StallGuardThreshold>63)   StallGuardConfig[Which262].StallGuardThreshold=63;
  if(StallGuardConfig[Which262].StallGuardThreshold<-63)  StallGuardConfig[Which262].StallGuardThreshold=-63;
  if(StallGuardConfig[Which262].CurrentScale>31)          StallGuardConfig[Which262].CurrentScale=31;

  SPIWriteInt=0;
  SPIWriteInt|=0x000c0000;                                //Registeradresse SGSCONF
  if(StallGuardConfig[Which262].FilterEnable==1) SPIWriteInt|=0X00010000;    //ģʽ
  SPIWriteInt|=((u32) StallGuardConfig[Which262].StallGuardThreshold & 0x7f) << 8;   //�ٽ�ֵ
  SPIWriteInt|=((u32) StallGuardConfig[Which262].CurrentScale);                      //����

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* �� �� ��     : WriteDriverConfig
* ��������		: ���ƼĴ���
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void WriteDriverConfig(u8 Which262)
{
  SPIWriteInt=0;
  SPIWriteInt|=0x000E0000;                     //Registeradresse DRVCONF
  SPIWriteInt|=((u32) DriverConfig[Which262].SlopeHighSide) << 14;
  SPIWriteInt|=((u32) DriverConfig[Which262].SlopeLowSide) << 12;
   
  if(DriverConfig[Which262].ProtectionDisable==1) SPIWriteInt|=0x00000400;
  SPIWriteInt|=((u32) DriverConfig[Which262].ProtectionTimer) << 8;
  if(DriverConfig[Which262].StepDirectionDisable==1) SPIWriteInt|=0x00000040;
  if(DriverConfig[Which262].VSenseScale==1) SPIWriteInt|=0x000000020;
  SPIWriteInt|=((u32) DriverConfig[Which262].ReadBackSelect) << 4;

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* �� �� ��     : Set262_EEPROM
* ��������		: 262��������
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void Set262_EEPROM(void)
{
   
  if(ReadEepromByte(0x12)!=55)
  {
  //�����洢��EEPROM
  WriteEepromBlock(0X200,(u8*)&StepDirConfig[0],3);
  WriteEepromBlock(0X230,(u8*)&StepDirConfig[1],3); 
  WriteEepromBlock(0X260,(u8*)&StepDirConfig[2],3);
  WriteEepromByte(0x12,55);
     
  WriteEepromBlock(0X330,(u8*)&StepDirConfig[0],3);  //��ֹ�ı䣬�ָ���������
  
  }
   
  ReadEepromBlock(0x200,(u8*)&StepDirConfig[0],3);
  ReadEepromBlock(0x230,(u8*)&StepDirConfig[1],3);
  ReadEepromBlock(0x260,(u8*)&StepDirConfig[2],3);

}
/*******************************************************************************
* �� �� ��     : InitMotorDrivers
* ��������		: ��ʼ��
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void InitMotorDrivers(void)
{
   u32 i;
   Set262_EEPROM();    //��ʼ������
   //��ʼ�����ݽṹ
  for(i=0; i<NUM_Motor; i++)
  {
    StallGuardConfig[i].FilterEnable=1;               //����ģʽ
    StallGuardConfig[i].StallGuardThreshold=5;        //ʧ�ټ��ֵ
    StallGuardConfig[i].CurrentScale=5;               //��������;   5   &&&&&&&&&&&&&&&&&&&&&&&

    DriverConfig[i].SlopeHighSide=3;                  //�¶Ȳ���
    DriverConfig[i].SlopeLowSide=3;                   //
    DriverConfig[i].ProtectionDisable=0;              //��·�ӵر���
    DriverConfig[i].ProtectionTimer=0;                //��·ʱ��
    DriverConfig[i].StepDirectionDisable=0;           //ģʽ
    DriverConfig[i].VSenseScale=1;
    DriverConfig[i].ReadBackSelect=TMC262_RB_SMART_ENERGY;     //���ݷ���ģʽ  ��ʧ�٣���������

    SmartEnergyControl[i].SmartIMin=0;                //CS��Сֵ    
    SmartEnergyControl[i].SmartDownStep=0;            //��ֵ����˥��
    SmartEnergyControl[i].SmartStallLevelMax=0;       //
    SmartEnergyControl[i].SmartUpStep=0;              //
    SmartEnergyControl[i].SmartStallLevelMin=0;       //

 //   StepDirConfig[i].Intpol=0;                      //�����ֵ���ر�
 //   StepDirConfig[i].DEdge=0;                       //����ѡ�� ,������
 //   StepDirConfig[i].MRes=3;                        //ϸ������%0000: 256 %0001: 128 %0010: 64  %0011: 32  
                                                      //%0100: 16   %0101: 8   %0110: 4      %0111: 2 (halfstep)  %1000: 1 (fullstep)
        
    ChopperConfig[i].BlankTime=2;                     //����ʱ����
    ChopperConfig[i].ChopperMode=0;                   //��׼ģʽ������˥��ģʽ
    ChopperConfig[i].RandomTOff=0;                    //�̶�ģʽ
    ChopperConfig[i].HysteresisDecay=0;               //����˥��
    ChopperConfig[i].HysteresisEnd=2;                 //�ͺ�
    ChopperConfig[i].HysteresisStart=3;                        
    ChopperConfig[i].TOff=5;                          //NCLK= 12 + (32 x TOFF) ,��˥��ʱ��  5
    ChopperConfig[i].DisableFlag=FALSE;               //ն����־�ر�
  }

  ENABLE_DRIVERS=0;   //ʹ��262 ����
  
  
  
  for(i=0; i<NUM_Motor; i++)                   //��ʼ������
  {
    WriteSmartEnergyControl(i);
    WriteStallGuardConfig(i);
    WriteDriverConfig(i);
    WriteStepDirConfig(i);
    WriteChopperConfig(i);
  }
}
/*******************************************************************************
* �� �� ��     : Set262StepDirMStepRes
* ��������		: ϸ������
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void Set262StepDirMStepRes(u8 Which262, u8 MicrostepResolution)     //ϸ������
{
  StepDirConfig[Which262].MRes=MicrostepResolution;
  if(MicrostepResolution!=4) StepDirConfig[Which262].Intpol=0;

  WriteStepDirConfig(Which262);
}
/*******************************************************************************
* �� �� ��     : Set262StepDirInterpolation
* ��������		: ��ֵģʽ����16΢��ʱ��Ӧ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void Set262StepDirInterpolation(u8 Which262, u8 Interpolation)    //
{
  StepDirConfig[Which262].Intpol=Interpolation;
  if(Interpolation) StepDirConfig[Which262].MRes=4;

  WriteStepDirConfig(Which262);
}

/*******************************************************************************
* �� �� ��     : Set262StepDirDoubleEdge
* ��������		: ΢������ѡ��
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void Set262StepDirDoubleEdge(u8 Which262, u8 DoubleEdge)       //΢������ѡ��
{
  StepDirConfig[Which262].DEdge=DoubleEdge;

  WriteStepDirConfig(Which262);
}

/*******************************************************************************
* �� �� ��     : Get262StepDirMStepRes
* ��������		: �õ�262   ϸ�֣��ֱ���
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
u8 Get262StepDirMStepRes(u8 Which262)     //�õ�262   ϸ�֣��ֱ���
{
  return StepDirConfig[Which262].MRes;
}

/*******************************************************************************
* �� �� ��     : Get262StepDirInterpolation
* ��������		: ����ֵģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
u8 Get262StepDirInterpolation(u8 Which262)      //����ֵģʽ
{
  return StepDirConfig[Which262].Intpol;
}

/*******************************************************************************
* �� �� ��     : Get262StepDirDoubleEdge
* ��������		: �õ�΢������ģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
u8 Get262StepDirDoubleEdge(u8 Which262)
{
  return StepDirConfig[Which262].DEdge;
}

/*******************************************************************************
* �� �� ��     : Set262ChopperBlankTime
* ��������		: ն������
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void Set262ChopperBlankTime(u8 Which262, u8 BlankTime)    //����ն���հ�ʱ��
{
  ChopperConfig[Which262].BlankTime=BlankTime;
  WriteChopperConfig(Which262);
}

/*******************************************************************************
* �� �� ��     : Set262ChopperMode
* ��������		: ն��ģʽ
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void Set262ChopperMode(u8 Which262, u8 Mode)   //����ն��ģʽ
{
  ChopperConfig[Which262].ChopperMode=Mode;
  WriteChopperConfig(Which262);
}

/*******************************************************************************
* �� �� ��     : Set262ChopperHysteresisDecay
* ��������		: ն������
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/

void Set262ChopperHysteresisDecay(u8 Which262, u8 HysteresisDecay)    //
{
  ChopperConfig[Which262].HysteresisDecay=HysteresisDecay;
  WriteChopperConfig(Which262);
}

void Set262ChopperHysteresisEnd(u8 Which262, u8 HysteresisEnd)       //�ͺ����
{
  ChopperConfig[Which262].HysteresisEnd=HysteresisEnd;
  WriteChopperConfig(Which262);
}

void Set262ChopperHysteresisStart(u8 Which262, u8 HysteresisStart)    //��ʼ����
{
  ChopperConfig[Which262].HysteresisStart=HysteresisStart;
  WriteChopperConfig(Which262);
}

void Set262ChopperTOff(u8 Which262, u8 TOff)                          //����ʱ��
{
  ChopperConfig[Which262].TOff=TOff;
  WriteChopperConfig(Which262);
}

//////////////////////////////////////////////////
void Set262SmartEnergyIMin(u8 Which262, u8 SmartIMin)                 //����ģʽ
{
  SmartEnergyControl[Which262].SmartIMin=SmartIMin;
  WriteSmartEnergyControl(Which262);
}

void Set262StallGuardCurrentScale(u8 Which262, u8 CurrentScale)       //������С
{
  StallGuardConfig[Which262].CurrentScale=CurrentScale;
  WriteStallGuardConfig(Which262);
}

void Set262DriverStepDirectionOff(u8 Which262, u8 SDOff)              //����ģʽ
{
  DriverConfig[Which262].StepDirectionDisable=SDOff;
  WriteDriverConfig(Which262);
}

void Set262DriverVSenseScale(u8 Which262, u8 Scale)                   //�ο���ѹ
{
  DriverConfig[Which262].VSenseScale=Scale;
  WriteDriverConfig(Which262);
}

void Set262DriverReadSelect(u8 Which262, u8 ReadSelect)               //���ݶ��ط�ʽ
{
  DriverConfig[Which262].ReadBackSelect=ReadSelect;
  WriteDriverConfig(Which262);
}
/*******************************************************************************
* �� �� ��     : Read262State
* ��������		: ��ʼ��
* ��    ��     : ���
* ��    ��     : ��
*   This function reads out the actual status information of a TMC26x.
*   The data will be decoded according to the read back mode that has
*   been set using Set262DriverReadSelect().
*   For values that are not needed NULL pointers can be specified.
*******************************************************************************/
void Read262State(u8 Which262, u8 *Phases, u8 *MStep, u32 *StallGuard, u8 *SmartEnergy, u8 *Flags)
{
  //Abwechselnd alle Datagrammtypen des TMC26x zum Auslesen der Zust�nde benutzen.
  //So wird sichergestellt, da� alle Register immer mit den richtigen Werten beschrieben
  //sind (falls es zwischendurch mal zu einem Reset des TMC26x kommen sollte).
   
   
  switch(ReadBackDatagram[Which262])
  {
    case RB_CHOPPER:
      WriteChopperConfig(Which262);
      ReadBackDatagram[Which262]=RB_DRIVER;
      break;

    case RB_DRIVER:
      WriteDriverConfig(Which262);
      ReadBackDatagram[Which262]=RB_SMART_ENERGY;
      break;

    case RB_SMART_ENERGY:
      WriteSmartEnergyControl(Which262);
      ReadBackDatagram[Which262]=RB_STALL_GUARD;
      break;

    case RB_STALL_GUARD:
      WriteStallGuardConfig(Which262);
      ReadBackDatagram[Which262]=RB_STEP_DIR;
      break;

    case RB_STEP_DIR:
      WriteStepDirConfig(Which262);
      ReadBackDatagram[Which262]=RB_CHOPPER;
      break;

    default:
      ReadBackDatagram[Which262]=RB_CHOPPER;
      break;
  }

  //Decode the read back values
  switch(DriverConfig[Which262].ReadBackSelect)
  {
    case TMC262_RB_MSTEP:                                  //΢��λ��
      if(Phases!=NULL) *Phases=SPIReadInt >> 18;           //����
      if(MStep!=NULL)  *MStep=SPIReadInt >> 10;            //΢��ֵ
      break;

    case TMC262_RB_STALL_GUARD:
      if(StallGuard!=NULL) *StallGuard=SPIReadInt >> 10;   //SPI ģʽ�µ�ֵSG ֵ
      break;

    case TMC262_RB_SMART_ENERGY:                           
      if(StallGuard!=NULL)
      {
        *StallGuard=SPIReadInt >> 15;                       //SPI ģʽ�µ�SE ֵ
        *StallGuard<<=5;
      }
      if(SmartEnergy!=NULL)                                 //
      {
        *SmartEnergy=(SPIReadInt >> 10) & 0x1f;             //SE ֵ 5 λ
      }
      break;
  }

  if(Flags!=NULL)   *Flags=SPIReadInt & 0xff;               //�����ݱ�־λ
}
/*******************************************************************************
* �� �� ��     : Disable262
* ��������		: ʹ��
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void Disable262(u8 Which262)
{
  if(!ChopperConfig[Which262].DisableFlag)
  {
    ChopperConfig[Which262].DisableFlag=TRUE;
    WriteChopperConfig(Which262);                     //����
  }
}


/***************************************************************//**
   \fn Enable262(u8 Which262)
   \brief Enable a motor driver
   \param Which262  Axis number (always 0 on stepRocker)

   Re-enable a motor driver (by setting its TOff value back to the
   original value).
********************************************************************/
void Enable262(u8 Which262)
{
  if(ChopperConfig[Which262].DisableFlag)
  {
    ChopperConfig[Which262].DisableFlag=FALSE;
    WriteChopperConfig(Which262);                    //����
  }
}

/*******************************************************************************
* �� �� ��     : SetMotorCurrent
* ��������		: ���õ���
* ��    ��     : ���
* ��    ��     : ��
*******************************************************************************/
void SetMotorCurrent(u8 Motor, u8 Current)
{
  Set262StallGuardCurrentScale(WHICH_262(Motor), Current>>3);
}


u8  Get262SmartEnergyStallLevelMin(u8 Which262)
{
  return SmartEnergyControl[Which262].SmartStallLevelMin;
}

void Set262SmartEnergyStallLevelMin(u8 Which262, u8 StallLevelMin)
{
  SmartEnergyControl[Which262].SmartStallLevelMin=StallLevelMin;
  WriteSmartEnergyControl(Which262);
}




