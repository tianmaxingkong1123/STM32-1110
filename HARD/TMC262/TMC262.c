/**********************************************************************
* Copyright (c) 2002-2016 É½¶«²©¿ÆÉúÎï²úÒµÓÐÏÞ¹«Ë¾ All rights reserved.
*
* ÎÄ¼þÃû³Æ£º TMC429.c
* Ãè    Êö£º TMC429Ïà¹Ø³ÌÐò£¬°üº¬³õÊ¼»¯,¶ÁÐ´º¯Êý
*
* ÐÞ¸Ä¼ÇÂ¼£º
*
* V1.0£¬2006.12.22£¬ÇÇ±ò
**********************************************************************/
#include "TMC262.h"
#include "SPI.h"
#include "Globals.h"
#include "at25128b.h"


/*******************************************************************************
* º¯ Êý Ãû     : ReadWrite262
* º¯Êý¹¦ÄÜ		: 429 ¶ÁÐ´º¯Êý,32Î»
* Êä    Èë     : ÎÞ
* Êä    ³ö     : ÎÞ
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
     //ºóÐøÌí¼Ó262      
   }
   else
    {    
     //ºóÐøÀ©Õ¹262
    }
}

/*******************************************************************************
* º¯ Êý Ãû     : WriteStepDirConfig
* º¯Êý¹¦ÄÜ		: 262  Ä£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/

void WriteStepDirConfig(u8 Which262)
{
  SPIWriteInt=0;
  if(StepDirConfig[Which262].Intpol)   SPIWriteInt|=0x00000200;     
  if(StepDirConfig[Which262].DEdge)    SPIWriteInt|=0x00000100;
  if(StepDirConfig[Which262].MRes>15)  StepDirConfig[Which262].MRes=15;    //Ï¸·Ö£¬×î´ó
 
  SPIWriteInt|=StepDirConfig[Which262].MRes;                               //µØÖ·0000

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);                        //Ð´262¼Ä´æÆ÷
}
/*******************************************************************************
* º¯ Êý Ãû     : WriteStepDirConfig
* º¯Êý¹¦ÄÜ		: Õ¶²¨Ä£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
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
  if(ChopperConfig[Which262].ChopperMode) SPIWriteInt|=0x00004000;    //mode Î»   14
  if(ChopperConfig[Which262].RandomTOff) SPIWriteInt|=0x00002000;     //ram  Î»   13
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisDecay) << 11;
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisEnd) << 7;
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisStart) << 4;
   
  if(!ChopperConfig[Which262].DisableFlag)                      //²»¹Ø±Õ
      SPIWriteInt|=((u32)  ChopperConfig[Which262].TOff);       //wenn DisableFlag gesetzt wird 0 gesendet

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}

/*******************************************************************************
* º¯ Êý Ãû     : WriteSmartEnergyControl
* º¯Êý¹¦ÄÜ		: µçÁ÷Ä£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
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
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartIMin) << 15;         //×îÐ¡Öµ
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartDownStep)  << 13;    //Ë¥¼õËÙ¶È
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartStallLevelMax) << 8; //
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartUpStep) << 5;        //ÔöÁ¿´óÐ¡
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartStallLevelMin);       

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* º¯ Êý Ãû     : WriteSmartEnergyControl
* º¯Êý¹¦ÄÜ		: µçÁ÷Ä£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void WriteStallGuardConfig(u8 Which262)
{
  if(StallGuardConfig[Which262].StallGuardThreshold>63)   StallGuardConfig[Which262].StallGuardThreshold=63;
  if(StallGuardConfig[Which262].StallGuardThreshold<-63)  StallGuardConfig[Which262].StallGuardThreshold=-63;
  if(StallGuardConfig[Which262].CurrentScale>31)          StallGuardConfig[Which262].CurrentScale=31;

  SPIWriteInt=0;
  SPIWriteInt|=0x000c0000;                                //Registeradresse SGSCONF
  if(StallGuardConfig[Which262].FilterEnable==1) SPIWriteInt|=0X00010000;    //Ä£Ê½
  SPIWriteInt|=((u32) StallGuardConfig[Which262].StallGuardThreshold & 0x7f) << 8;   //ÁÙ½çÖµ
  SPIWriteInt|=((u32) StallGuardConfig[Which262].CurrentScale);                      //µçÁ÷

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* º¯ Êý Ãû     : WriteDriverConfig
* º¯Êý¹¦ÄÜ		: ¿ØÖÆ¼Ä´æÆ÷
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
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
* º¯ Êý Ãû     : Set262_EEPROM
* º¯Êý¹¦ÄÜ		: 262²ÎÊýÉèÖÃ
* Êä    Èë     : ÎÞ
* Êä    ³ö     : ÎÞ
*******************************************************************************/

void Set262_EEPROM(void)
{
   
  if(ReadEepromByte(0x12)!=55)
  {
  //²ÎÊý´æ´¢µ½EEPROM
  WriteEepromBlock(0X200,(u8*)&StepDirConfig[0],3);
  WriteEepromBlock(0X230,(u8*)&StepDirConfig[1],3); 
  WriteEepromBlock(0X260,(u8*)&StepDirConfig[2],3);
  WriteEepromByte(0x12,55);
     
  WriteEepromBlock(0X330,(u8*)&StepDirConfig[0],3);  //½ûÖ¹¸Ä±ä£¬»Ö¸´³ö³§ÉèÖÃ
  
  }
   
  ReadEepromBlock(0x200,(u8*)&StepDirConfig[0],3);
  ReadEepromBlock(0x230,(u8*)&StepDirConfig[1],3);
  ReadEepromBlock(0x260,(u8*)&StepDirConfig[2],3);

}
/*******************************************************************************
* º¯ Êý Ãû     : InitMotorDrivers
* º¯Êý¹¦ÄÜ		: ³õÊ¼»¯
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void InitMotorDrivers(void)
{
   u32 i;
   Set262_EEPROM();    //³õÊ¼»¯²ÎÊý
   //³õÊ¼»¯Êý¾Ý½á¹¹
  for(i=0; i<NUM_Motor; i++)
  {
    StallGuardConfig[i].FilterEnable=1;               //¹ýÂËÄ£Ê½
    StallGuardConfig[i].StallGuardThreshold=5;        //Ê§ËÙ¼ì²âÖµ
    StallGuardConfig[i].CurrentScale=5;               //µçÁ÷±ÈÀý;   5   &&&&&&&&&&&&&&&&&&&&&&&

    DriverConfig[i].SlopeHighSide=3;                  //ÎÂ¶È²¹³¥
    DriverConfig[i].SlopeLowSide=3;                   //
    DriverConfig[i].ProtectionDisable=0;              //¶ÌÂ·½ÓµØ±£»¤
    DriverConfig[i].ProtectionTimer=0;                //¶ÌÂ·Ê±¼ä
    DriverConfig[i].StepDirectionDisable=0;           //Ä£Ê½
    DriverConfig[i].VSenseScale=1;
    DriverConfig[i].ReadBackSelect=TMC262_RB_SMART_ENERGY;     //Êý¾Ý·µ»ØÄ£Ê½  £¬Ê§ËÙ£¬µçÁ÷·µ»Ø

    SmartEnergyControl[i].SmartIMin=0;                //CS×îÐ¡Öµ    
    SmartEnergyControl[i].SmartDownStep=0;            //ãÐÖµµçÁ÷Ë¥¼õ
    SmartEnergyControl[i].SmartStallLevelMax=0;       //
    SmartEnergyControl[i].SmartUpStep=0;              //
    SmartEnergyControl[i].SmartStallLevelMin=0;       //

 //   StepDirConfig[i].Intpol=0;                      //Âö³å²åÖµ£¬¹Ø±Õ
 //   StepDirConfig[i].DEdge=0;                       //±ßÑØÑ¡Ôñ ,ÉÏÉýÑØ
 //   StepDirConfig[i].MRes=3;                        //Ï¸·ÖÉèÖÃ%0000: 256 %0001: 128 %0010: 64  %0011: 32  
                                                      //%0100: 16   %0101: 8   %0110: 4      %0111: 2 (halfstep)  %1000: 1 (fullstep)
        
    ChopperConfig[i].BlankTime=2;                     //ÏûÒþÊ±¼ä¼ä¸ô
    ChopperConfig[i].ChopperMode=0;                   //±ê×¼Ä£Ê½£¬¿ìËÙË¥¼õÄ£Ê½
    ChopperConfig[i].RandomTOff=0;                    //¹Ì¶¨Ä£Ê½
    ChopperConfig[i].HysteresisDecay=0;               //´ÅÖÍË¥¼õ
    ChopperConfig[i].HysteresisEnd=2;                 //ÖÍºó
    ChopperConfig[i].HysteresisStart=3;                        
    ChopperConfig[i].TOff=5;                          //NCLK= 12 + (32 x TOFF) ,ÂýË¥¼õÊ±¼ä  5
    ChopperConfig[i].DisableFlag=FALSE;               //Õ¶²¨±êÖ¾¹Ø±Õ
  }

  ENABLE_DRIVERS=0;   //Ê¹ÄÜ262 Òý½Å
  
  
  
  for(i=0; i<NUM_Motor; i++)                   //³õÊ¼»¯Æ÷¼þ
  {
    WriteSmartEnergyControl(i);
    WriteStallGuardConfig(i);
    WriteDriverConfig(i);
    WriteStepDirConfig(i);
    WriteChopperConfig(i);
  }
}
/*******************************************************************************
* º¯ Êý Ãû     : Set262StepDirMStepRes
* º¯Êý¹¦ÄÜ		: Ï¸·ÖÉèÖÃ
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void Set262StepDirMStepRes(u8 Which262, u8 MicrostepResolution)     //Ï¸·ÖÉèÖÃ
{
  StepDirConfig[Which262].MRes=MicrostepResolution;
  if(MicrostepResolution!=4) StepDirConfig[Which262].Intpol=0;

  WriteStepDirConfig(Which262);
}
/*******************************************************************************
* º¯ Êý Ãû     : Set262StepDirInterpolation
* º¯Êý¹¦ÄÜ		: ²åÖµÄ£Ê½£¬£¬16Î¢²½Ê±ÊÊÓ¦
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void Set262StepDirInterpolation(u8 Which262, u8 Interpolation)    //
{
  StepDirConfig[Which262].Intpol=Interpolation;
  if(Interpolation) StepDirConfig[Which262].MRes=4;

  WriteStepDirConfig(Which262);
}

/*******************************************************************************
* º¯ Êý Ãû     : Set262StepDirDoubleEdge
* º¯Êý¹¦ÄÜ		: Î¢²½±ßÑØÑ¡Ôñ
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void Set262StepDirDoubleEdge(u8 Which262, u8 DoubleEdge)       //Î¢²½±ßÑØÑ¡Ôñ
{
  StepDirConfig[Which262].DEdge=DoubleEdge;

  WriteStepDirConfig(Which262);
}

/*******************************************************************************
* º¯ Êý Ãû     : Get262StepDirMStepRes
* º¯Êý¹¦ÄÜ		: µÃµ½262   Ï¸·Ö£¬·Ö±æÂÊ
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
u8 Get262StepDirMStepRes(u8 Which262)     //µÃµ½262   Ï¸·Ö£¬·Ö±æÂÊ
{
  return StepDirConfig[Which262].MRes;
}

/*******************************************************************************
* º¯ Êý Ãû     : Get262StepDirInterpolation
* º¯Êý¹¦ÄÜ		: ¶Á²åÖµÄ£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
u8 Get262StepDirInterpolation(u8 Which262)      //¶Á²åÖµÄ£Ê½
{
  return StepDirConfig[Which262].Intpol;
}

/*******************************************************************************
* º¯ Êý Ãû     : Get262StepDirDoubleEdge
* º¯Êý¹¦ÄÜ		: µÃµ½Î¢²½±ßÑØÄ£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
u8 Get262StepDirDoubleEdge(u8 Which262)
{
  return StepDirConfig[Which262].DEdge;
}

/*******************************************************************************
* º¯ Êý Ãû     : Set262ChopperBlankTime
* º¯Êý¹¦ÄÜ		: Õ¶²¨²ÎÊý
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void Set262ChopperBlankTime(u8 Which262, u8 BlankTime)    //ÉèÖÃÕ¶²¨¿Õ°×Ê±¼ä
{
  ChopperConfig[Which262].BlankTime=BlankTime;
  WriteChopperConfig(Which262);
}

/*******************************************************************************
* º¯ Êý Ãû     : Set262ChopperMode
* º¯Êý¹¦ÄÜ		: Õ¶²¨Ä£Ê½
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void Set262ChopperMode(u8 Which262, u8 Mode)   //ÉèÖÃÕ¶²¨Ä£Ê½
{
  ChopperConfig[Which262].ChopperMode=Mode;
  WriteChopperConfig(Which262);
}

/*******************************************************************************
* º¯ Êý Ãû     : Set262ChopperHysteresisDecay
* º¯Êý¹¦ÄÜ		: Õ¶²¨²ÎÊý
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/

void Set262ChopperHysteresisDecay(u8 Which262, u8 HysteresisDecay)    //
{
  ChopperConfig[Which262].HysteresisDecay=HysteresisDecay;
  WriteChopperConfig(Which262);
}

void Set262ChopperHysteresisEnd(u8 Which262, u8 HysteresisEnd)       //ÖÍºó²ÎÊý
{
  ChopperConfig[Which262].HysteresisEnd=HysteresisEnd;
  WriteChopperConfig(Which262);
}

void Set262ChopperHysteresisStart(u8 Which262, u8 HysteresisStart)    //¿ªÊ¼²ÎÊý
{
  ChopperConfig[Which262].HysteresisStart=HysteresisStart;
  WriteChopperConfig(Which262);
}

void Set262ChopperTOff(u8 Which262, u8 TOff)                          //³¬³öÊ±¼ä
{
  ChopperConfig[Which262].TOff=TOff;
  WriteChopperConfig(Which262);
}

//////////////////////////////////////////////////
void Set262SmartEnergyIMin(u8 Which262, u8 SmartIMin)                 //µçÁ÷Ä£Ê½
{
  SmartEnergyControl[Which262].SmartIMin=SmartIMin;
  WriteSmartEnergyControl(Which262);
}

void Set262StallGuardCurrentScale(u8 Which262, u8 CurrentScale)       //µçÁ÷´óÐ¡
{
  StallGuardConfig[Which262].CurrentScale=CurrentScale;
  WriteStallGuardConfig(Which262);
}

void Set262DriverStepDirectionOff(u8 Which262, u8 SDOff)              //ÊäÈëÄ£Ê½
{
  DriverConfig[Which262].StepDirectionDisable=SDOff;
  WriteDriverConfig(Which262);
}

void Set262DriverVSenseScale(u8 Which262, u8 Scale)                   //²Î¿¼µçÑ¹
{
  DriverConfig[Which262].VSenseScale=Scale;
  WriteDriverConfig(Which262);
}

void Set262DriverReadSelect(u8 Which262, u8 ReadSelect)               //Êý¾Ý¶Á»Ø·½Ê½
{
  DriverConfig[Which262].ReadBackSelect=ReadSelect;
  WriteDriverConfig(Which262);
}
/*******************************************************************************
* º¯ Êý Ãû     : Read262State
* º¯Êý¹¦ÄÜ		: ³õÊ¼»¯
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*   This function reads out the actual status information of a TMC26x.
*   The data will be decoded according to the read back mode that has
*   been set using Set262DriverReadSelect().
*   For values that are not needed NULL pointers can be specified.
*******************************************************************************/
void Read262State(u8 Which262, u8 *Phases, u8 *MStep, u32 *StallGuard, u8 *SmartEnergy, u8 *Flags)
{
  //Abwechselnd alle Datagrammtypen des TMC26x zum Auslesen der Zustände benutzen.
  //So wird sichergestellt, daß alle Register immer mit den richtigen Werten beschrieben
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
    case TMC262_RB_MSTEP:                                  //Î¢²½Î»ÖÃ
      if(Phases!=NULL) *Phases=SPIReadInt >> 18;           //¼«ÐÔ
      if(MStep!=NULL)  *MStep=SPIReadInt >> 10;            //Î¢²½Öµ
      break;

    case TMC262_RB_STALL_GUARD:
      if(StallGuard!=NULL) *StallGuard=SPIReadInt >> 10;   //SPI Ä£Ê½ÏÂµÄÖµSG Öµ
      break;

    case TMC262_RB_SMART_ENERGY:                           
      if(StallGuard!=NULL)
      {
        *StallGuard=SPIReadInt >> 15;                       //SPI Ä£Ê½ÏÂµÄSE Öµ
        *StallGuard<<=5;
      }
      if(SmartEnergy!=NULL)                                 //
      {
        *SmartEnergy=(SPIReadInt >> 10) & 0x1f;             //SE Öµ 5 Î»
      }
      break;
  }

  if(Flags!=NULL)   *Flags=SPIReadInt & 0xff;               //¶ÁÊý¾Ý±êÖ¾Î»
}
/*******************************************************************************
* º¯ Êý Ãû     : Disable262
* º¯Êý¹¦ÄÜ		: Ê¹ÄÜ
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
*******************************************************************************/
void Disable262(u8 Which262)
{
  if(!ChopperConfig[Which262].DisableFlag)
  {
    ChopperConfig[Which262].DisableFlag=TRUE;
    WriteChopperConfig(Which262);                     //¿ªÆô
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
    WriteChopperConfig(Which262);                    //¿ªÆô
  }
}

/*******************************************************************************
* º¯ Êý Ãû     : SetMotorCurrent
* º¯Êý¹¦ÄÜ		: ÉèÖÃµçÁ÷
* Êä    Èë     : ÐòºÅ
* Êä    ³ö     : ÎÞ
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




