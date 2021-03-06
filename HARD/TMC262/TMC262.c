/**********************************************************************
* Copyright (c) 2002-2016 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： TMC429.c
* 描    述： TMC429相关程序，包含初始化,读写函数
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
**********************************************************************/
#include "TMC262.h"
#include "SPI.h"
#include "Globals.h"
#include "at25128b.h"


/*******************************************************************************
* 函 数 名     : ReadWrite262
* 函数功能		: 429 读写函数,32位
* 输    入     : 无
* 输    出     : 无
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
     //后续添加262      
   }
   else
    {    
     //后续扩展262
    }
}

/*******************************************************************************
* 函 数 名     : WriteStepDirConfig
* 函数功能		: 262  模式
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/

void WriteStepDirConfig(u8 Which262)
{
  SPIWriteInt=0;
  if(StepDirConfig[Which262].Intpol)   SPIWriteInt|=0x00000200;     
  if(StepDirConfig[Which262].DEdge)    SPIWriteInt|=0x00000100;
  if(StepDirConfig[Which262].MRes>15)  StepDirConfig[Which262].MRes=15;    //细分，最大
 
  SPIWriteInt|=StepDirConfig[Which262].MRes;                               //地址0000

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);                        //写262寄存器
}
/*******************************************************************************
* 函 数 名     : WriteStepDirConfig
* 函数功能		: 斩波模式
* 输    入     : 序号
* 输    出     : 无
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
  if(ChopperConfig[Which262].ChopperMode) SPIWriteInt|=0x00004000;    //mode 位   14
  if(ChopperConfig[Which262].RandomTOff) SPIWriteInt|=0x00002000;     //ram  位   13
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisDecay) << 11;
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisEnd) << 7;
  SPIWriteInt|=((u32) ChopperConfig[Which262].HysteresisStart) << 4;
   
  if(!ChopperConfig[Which262].DisableFlag)                      //不关闭
      SPIWriteInt|=((u32)  ChopperConfig[Which262].TOff);       //wenn DisableFlag gesetzt wird 0 gesendet

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}

/*******************************************************************************
* 函 数 名     : WriteSmartEnergyControl
* 函数功能		: 电流模式
* 输    入     : 序号
* 输    出     : 无
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
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartIMin) << 15;         //最小值
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartDownStep)  << 13;    //衰减速度
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartStallLevelMax) << 8; //
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartUpStep) << 5;        //增量大小
  SPIWriteInt|=((u32) SmartEnergyControl[Which262].SmartStallLevelMin);       

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* 函 数 名     : WriteSmartEnergyControl
* 函数功能		: 电流模式
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void WriteStallGuardConfig(u8 Which262)
{
  if(StallGuardConfig[Which262].StallGuardThreshold>63)   StallGuardConfig[Which262].StallGuardThreshold=63;
  if(StallGuardConfig[Which262].StallGuardThreshold<-63)  StallGuardConfig[Which262].StallGuardThreshold=-63;
  if(StallGuardConfig[Which262].CurrentScale>31)          StallGuardConfig[Which262].CurrentScale=31;

  SPIWriteInt=0;
  SPIWriteInt|=0x000c0000;                                //Registeradresse SGSCONF
  if(StallGuardConfig[Which262].FilterEnable==1) SPIWriteInt|=0X00010000;    //模式
  SPIWriteInt|=((u32) StallGuardConfig[Which262].StallGuardThreshold & 0x7f) << 8;   //临界值
  SPIWriteInt|=((u32) StallGuardConfig[Which262].CurrentScale);                      //电流

  ReadWrite262(Which262, &SPIReadInt, SPIWriteInt);
}
/*******************************************************************************
* 函 数 名     : WriteDriverConfig
* 函数功能		: 控制寄存器
* 输    入     : 序号
* 输    出     : 无
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
* 函 数 名     : Set262_EEPROM
* 函数功能		: 262参数设置
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/

void Set262_EEPROM(void)
{
   
  if(ReadEepromByte(0x12)!=55)
  {
  //参数存储到EEPROM
  WriteEepromBlock(0X200,(u8*)&StepDirConfig[0],3);
  WriteEepromBlock(0X230,(u8*)&StepDirConfig[1],3); 
  WriteEepromBlock(0X260,(u8*)&StepDirConfig[2],3);
  WriteEepromByte(0x12,55);
     
  WriteEepromBlock(0X330,(u8*)&StepDirConfig[0],3);  //禁止改变，恢复出厂设置
  
  }
   
  ReadEepromBlock(0x200,(u8*)&StepDirConfig[0],3);
  ReadEepromBlock(0x230,(u8*)&StepDirConfig[1],3);
  ReadEepromBlock(0x260,(u8*)&StepDirConfig[2],3);

}
/*******************************************************************************
* 函 数 名     : InitMotorDrivers
* 函数功能		: 初始化
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void InitMotorDrivers(void)
{
   u32 i;
   Set262_EEPROM();    //初始化参数
   //初始化数据结构
  for(i=0; i<NUM_Motor; i++)
  {
    StallGuardConfig[i].FilterEnable=1;               //过滤模式
    StallGuardConfig[i].StallGuardThreshold=5;        //失速检测值
    StallGuardConfig[i].CurrentScale=5;               //电流比例;   5   &&&&&&&&&&&&&&&&&&&&&&&

    DriverConfig[i].SlopeHighSide=3;                  //温度补偿
    DriverConfig[i].SlopeLowSide=3;                   //
    DriverConfig[i].ProtectionDisable=0;              //短路接地保护
    DriverConfig[i].ProtectionTimer=0;                //短路时间
    DriverConfig[i].StepDirectionDisable=0;           //模式
    DriverConfig[i].VSenseScale=1;
    DriverConfig[i].ReadBackSelect=TMC262_RB_SMART_ENERGY;     //数据返回模式  ，失速，电流返回

    SmartEnergyControl[i].SmartIMin=0;                //CS最小值    
    SmartEnergyControl[i].SmartDownStep=0;            //阈值电流衰减
    SmartEnergyControl[i].SmartStallLevelMax=0;       //
    SmartEnergyControl[i].SmartUpStep=0;              //
    SmartEnergyControl[i].SmartStallLevelMin=0;       //

 //   StepDirConfig[i].Intpol=0;                      //脉冲插值，关闭
 //   StepDirConfig[i].DEdge=0;                       //边沿选择 ,上升沿
 //   StepDirConfig[i].MRes=3;                        //细分设置%0000: 256 %0001: 128 %0010: 64  %0011: 32  
                                                      //%0100: 16   %0101: 8   %0110: 4      %0111: 2 (halfstep)  %1000: 1 (fullstep)
        
    ChopperConfig[i].BlankTime=2;                     //消隐时间间隔
    ChopperConfig[i].ChopperMode=0;                   //标准模式，快速衰减模式
    ChopperConfig[i].RandomTOff=0;                    //固定模式
    ChopperConfig[i].HysteresisDecay=0;               //磁滞衰减
    ChopperConfig[i].HysteresisEnd=2;                 //滞后
    ChopperConfig[i].HysteresisStart=3;                        
    ChopperConfig[i].TOff=5;                          //NCLK= 12 + (32 x TOFF) ,慢衰减时间  5
    ChopperConfig[i].DisableFlag=FALSE;               //斩波标志关闭
  }

  ENABLE_DRIVERS=0;   //使能262 引脚
  
  
  
  for(i=0; i<NUM_Motor; i++)                   //初始化器件
  {
    WriteSmartEnergyControl(i);
    WriteStallGuardConfig(i);
    WriteDriverConfig(i);
    WriteStepDirConfig(i);
    WriteChopperConfig(i);
  }
}
/*******************************************************************************
* 函 数 名     : Set262StepDirMStepRes
* 函数功能		: 细分设置
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void Set262StepDirMStepRes(u8 Which262, u8 MicrostepResolution)     //细分设置
{
  StepDirConfig[Which262].MRes=MicrostepResolution;
  if(MicrostepResolution!=4) StepDirConfig[Which262].Intpol=0;

  WriteStepDirConfig(Which262);
}
/*******************************************************************************
* 函 数 名     : Set262StepDirInterpolation
* 函数功能		: 插值模式，，16微步时适应
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void Set262StepDirInterpolation(u8 Which262, u8 Interpolation)    //
{
  StepDirConfig[Which262].Intpol=Interpolation;
  if(Interpolation) StepDirConfig[Which262].MRes=4;

  WriteStepDirConfig(Which262);
}

/*******************************************************************************
* 函 数 名     : Set262StepDirDoubleEdge
* 函数功能		: 微步边沿选择
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void Set262StepDirDoubleEdge(u8 Which262, u8 DoubleEdge)       //微步边沿选择
{
  StepDirConfig[Which262].DEdge=DoubleEdge;

  WriteStepDirConfig(Which262);
}

/*******************************************************************************
* 函 数 名     : Get262StepDirMStepRes
* 函数功能		: 得到262   细分，分辨率
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
u8 Get262StepDirMStepRes(u8 Which262)     //得到262   细分，分辨率
{
  return StepDirConfig[Which262].MRes;
}

/*******************************************************************************
* 函 数 名     : Get262StepDirInterpolation
* 函数功能		: 读插值模式
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
u8 Get262StepDirInterpolation(u8 Which262)      //读插值模式
{
  return StepDirConfig[Which262].Intpol;
}

/*******************************************************************************
* 函 数 名     : Get262StepDirDoubleEdge
* 函数功能		: 得到微步边沿模式
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
u8 Get262StepDirDoubleEdge(u8 Which262)
{
  return StepDirConfig[Which262].DEdge;
}

/*******************************************************************************
* 函 数 名     : Set262ChopperBlankTime
* 函数功能		: 斩波参数
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void Set262ChopperBlankTime(u8 Which262, u8 BlankTime)    //设置斩波空白时间
{
  ChopperConfig[Which262].BlankTime=BlankTime;
  WriteChopperConfig(Which262);
}

/*******************************************************************************
* 函 数 名     : Set262ChopperMode
* 函数功能		: 斩波模式
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void Set262ChopperMode(u8 Which262, u8 Mode)   //设置斩波模式
{
  ChopperConfig[Which262].ChopperMode=Mode;
  WriteChopperConfig(Which262);
}

/*******************************************************************************
* 函 数 名     : Set262ChopperHysteresisDecay
* 函数功能		: 斩波参数
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/

void Set262ChopperHysteresisDecay(u8 Which262, u8 HysteresisDecay)    //
{
  ChopperConfig[Which262].HysteresisDecay=HysteresisDecay;
  WriteChopperConfig(Which262);
}

void Set262ChopperHysteresisEnd(u8 Which262, u8 HysteresisEnd)       //滞后参数
{
  ChopperConfig[Which262].HysteresisEnd=HysteresisEnd;
  WriteChopperConfig(Which262);
}

void Set262ChopperHysteresisStart(u8 Which262, u8 HysteresisStart)    //开始参数
{
  ChopperConfig[Which262].HysteresisStart=HysteresisStart;
  WriteChopperConfig(Which262);
}

void Set262ChopperTOff(u8 Which262, u8 TOff)                          //超出时间
{
  ChopperConfig[Which262].TOff=TOff;
  WriteChopperConfig(Which262);
}

//////////////////////////////////////////////////
void Set262SmartEnergyIMin(u8 Which262, u8 SmartIMin)                 //电流模式
{
  SmartEnergyControl[Which262].SmartIMin=SmartIMin;
  WriteSmartEnergyControl(Which262);
}

void Set262StallGuardCurrentScale(u8 Which262, u8 CurrentScale)       //电流大小
{
  StallGuardConfig[Which262].CurrentScale=CurrentScale;
  WriteStallGuardConfig(Which262);
}

void Set262DriverStepDirectionOff(u8 Which262, u8 SDOff)              //输入模式
{
  DriverConfig[Which262].StepDirectionDisable=SDOff;
  WriteDriverConfig(Which262);
}

void Set262DriverVSenseScale(u8 Which262, u8 Scale)                   //参考电压
{
  DriverConfig[Which262].VSenseScale=Scale;
  WriteDriverConfig(Which262);
}

void Set262DriverReadSelect(u8 Which262, u8 ReadSelect)               //数据读回方式
{
  DriverConfig[Which262].ReadBackSelect=ReadSelect;
  WriteDriverConfig(Which262);
}
/*******************************************************************************
* 函 数 名     : Read262State
* 函数功能		: 初始化
* 输    入     : 序号
* 输    出     : 无
*   This function reads out the actual status information of a TMC26x.
*   The data will be decoded according to the read back mode that has
*   been set using Set262DriverReadSelect().
*   For values that are not needed NULL pointers can be specified.
*******************************************************************************/
void Read262State(u8 Which262, u8 *Phases, u8 *MStep, u32 *StallGuard, u8 *SmartEnergy, u8 *Flags)
{
  //Abwechselnd alle Datagrammtypen des TMC26x zum Auslesen der Zust鋘de benutzen.
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
    case TMC262_RB_MSTEP:                                  //微步位置
      if(Phases!=NULL) *Phases=SPIReadInt >> 18;           //极性
      if(MStep!=NULL)  *MStep=SPIReadInt >> 10;            //微步值
      break;

    case TMC262_RB_STALL_GUARD:
      if(StallGuard!=NULL) *StallGuard=SPIReadInt >> 10;   //SPI 模式下的值SG 值
      break;

    case TMC262_RB_SMART_ENERGY:                           
      if(StallGuard!=NULL)
      {
        *StallGuard=SPIReadInt >> 15;                       //SPI 模式下的SE 值
        *StallGuard<<=5;
      }
      if(SmartEnergy!=NULL)                                 //
      {
        *SmartEnergy=(SPIReadInt >> 10) & 0x1f;             //SE 值 5 位
      }
      break;
  }

  if(Flags!=NULL)   *Flags=SPIReadInt & 0xff;               //读数据标志位
}
/*******************************************************************************
* 函 数 名     : Disable262
* 函数功能		: 使能
* 输    入     : 序号
* 输    出     : 无
*******************************************************************************/
void Disable262(u8 Which262)
{
  if(!ChopperConfig[Which262].DisableFlag)
  {
    ChopperConfig[Which262].DisableFlag=TRUE;
    WriteChopperConfig(Which262);                     //开启
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
    WriteChopperConfig(Which262);                    //开启
  }
}

/*******************************************************************************
* 函 数 名     : SetMotorCurrent
* 函数功能		: 设置电流
* 输    入     : 序号
* 输    出     : 无
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




