/**********************************************************************
* Copyright (c) 2002-2016 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： TMC429.c
* 描    述： TMC429相关程序，包含初始化,读写函数
*
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
**********************************************************************/
#include "TMC429.h"
#include "Globals.h"
#include "SPI.h"
#include "TMC262.h"
#include "AT25128B.h"


/*******************************************************************************
* 函 数 名     : ReadWrite429
* 函数功能		: 429 读写函数,32位
* 输    入     : 读写指针
* 输    出     : 读回指针
*******************************************************************************/

void ReadWrite429(u8 *Read, u8 *Write)
{
   u8 Motor;
   Motor=Write[0]&0x60>>5;
   if(Motor<NUM_Motor)
   {
     if(!(Write[0] & TMC429_READ) && (Write[0]<0x60) && !MotorDisable[Motor]               
                                  && ((Write[0] & 0x9f)==IDX_XTARGET || (Write[0] & 0x9f)==IDX_VTARGET))   //写目标位置或速度
       {
         if(StandbyFlag[Motor])                                //锁住标志
         {
           SetMotorCurrent(Motor, MotorConfig[Motor].IRun);    //设置运动电流  262
           StandbyFlag[Motor]=FALSE;                           //解锁
         }

         if(FreewheelingActive[Motor])                         //空闲状态
         {
           Enable262(WHICH_262(Motor));                        //滑行状态下，关闭MOS
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
* 函 数 名     : Write429Bytes
* 函数功能		: 429 读写函数  一个字节
* 输    入     : 写数据
* 输    出     : 无
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
* 函 数 名     : Write429Datagram
* 函数功能		: 429 读写函数    写三个位 ，独立，
* 输    入     : 地址，高/中/低位
* 输    出     : 无
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
* 函 数 名     : Write429Zero
* 函数功能		: 429 读写函数    寄存器写0
* 输    入     : 地址/指令
* 输    出     : 无
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
* 函 数 名     : Write429Short
* 函数功能		: 429 读写函数    低16位写入
* 输    入     : 地址/指令
* 输    出     : 无
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
* 函 数 名     : Write429Int
* 函数功能		: 429 读写函数    低24位写入
* 输    入     : 地址/指令
* 输    出     : 无
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
* 函 数 名     : Read429Status
* 函数功能		: 429 读写函数    
* 输    入     : 无
* 输    出     : 读到的数据
*******************************************************************************/
u8 Read429Status(void)
{
  return ReadWriteSPI(SPI_DEV_TMC429, 0x01, TRUE);
}
/*******************************************************************************
* 函 数 名     : Read429Bytes
* 函数功能		: 429 读写函数    
* 输    入     : 无
* 输    出     : 读到的数据
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
* 函 数 名     : Read429SingleByte
* 函数功能		: 429 读写函数    
* 输    入     : 地址,要读取的自己位置
* 输    出     : 读到的数据
*******************************************************************************/
u8 Read429SingleByte(u8 Address, u8 Index)
{
  u8 Read[4], Write[4];

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  return Read[Index];
}
/*******************************************************************************
* 函 数 名     : Read429Short
* 函数功能		: 429 读写函数    
* 输    入     : 地址,
* 输    出     : 字节
*******************************************************************************/
int Read429Short(u8 Address)
{
  u8 Read[4], Write[4];
  int Result;

  Write[0]=Address|TMC429_READ;
  ReadWrite429(Read, Write);

  Result=(Read[2]<<8)|(Read[3]);
  if(Result & 0x00000800) Result|=0xfffff000;    //错误

  return Result;
}
/*******************************************************************************
* 函 数 名     : Read429Int
* 函数功能		: 429 读写函数    
* 输    入     : 地址,
* 输    出     : 三个字节，可用来读数位置
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
* 函 数 名     : Set429RampMode
* 函数功能		: 模式选择    
* 输    入     : 轴，模式
* 输    出     : 无
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
* 函 数 名     : Set429SwitchMode
* 函数功能		: 开关模式选择    
* 输    入     : 轴，模式
* 输    出     : 无
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
* 函 数 名     : SetAMax
* 函数功能		: 加速度设置   
* 输    入     : 电机，速度
* 输    出     : 无
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
  pulse_div=PulseRampDiv>>4;                                     //计算PULSE_DIV 
  ramp_div=PulseRampDiv & 0x0f;                                  //计算RAMP_DIV

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
* 函 数 名     : HardStop
* 函数功能		: 速度设置 ，停止电机 
* 输    入     : 电机，速度
* 输    出     : 无
*******************************************************************************/
void HardStop(u8 Motor)
{
  Set429RampMode(MOTOR_NUMBER(Motor), RM_VELOCITY);    //设置模式，
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VTARGET);  //当前速度和目标速度为0
  Write429Zero((MOTOR_NUMBER(Motor)<<5)|IDX_VACTUAL);
}
/*******************************************************************************
* 函 数 名     : Init429
* 函数功能		: 429 初始化  
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Init429(void)
{
  u8 addr;
  u8 Motor;
  Set429_EEPROM();             //提取轴参数
  for(Motor=0; Motor<3; Motor++)
  {
    for(addr=0; addr<=IDX_XLATCHED; addr++)         //初始化轴参数
      Write429Zero(addr|(Motor<<5));                //将轴参数设置为0
  }
                                      //限位信号反转，正常使用时需要取消
  Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_SDO_INT|IFCONF_EN_REFR);//|0x01  限位反转指令
  //Write429Int(IDX_IF_CONFIG_429, IFCONF_EN_SD|IFCONF_SDO_INT);   //|IFCONF_EN_REFR|IFCONF_SDO_INT);     //设置SD 模式，使能右开关
  Write429Datagram(IDX_SMGP, 0x00, 0x00, 0x02);                  //设置电机数量，为SPI 模式下

  //设置最大速度与加速度参数。
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR0, 0x00,             //分频设置，影响的是速度，或脉冲频率， 加速度参数，spi 模式下的分频                  
                     (MotorConfig[0].PulseDiv<<4)|(MotorConfig[0].RampDiv & 0x0f), 0x00); 
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR1, 0x00, 
                     (MotorConfig[1].PulseDiv<<4)|(MotorConfig[1].RampDiv & 0x0f), 0x00);
  Write429Datagram(IDX_PULSEDIV_RAMPDIV|MOTOR2, 0x00, 
                     (MotorConfig[2].PulseDiv<<4)|(MotorConfig[2].RampDiv & 0x0f), 0x00);
  
  //设置限位开关的模式，运动RAMP 模式，停止模式，参考模式，左右限位开关，此处没有启用右限位
  
  
  
  Write429Datagram(IDX_REFCONF_RM|MOTOR0, 0x00, 
                   STOP_REF|(MotorConfig[0].right_limit<<1)| MotorConfig[0].left_limit, RM_RAMP);   //限位开关模式选择 
  Write429Datagram(IDX_REFCONF_RM|MOTOR1, 0x00, 
                   STOP_REF|(MotorConfig[1].right_limit<<1)| MotorConfig[0].left_limit, RM_RAMP);   //限位开关模式选择 
  Write429Datagram(IDX_REFCONF_RM|MOTOR2, 0x00, 
                   STOP_REF|(MotorConfig[2].right_limit<<1)| MotorConfig[2].left_limit, RM_RAMP);   //限位开关模式选择 

  Write429Short(IDX_VMIN|MOTOR0, MotorConfig[0].mini_speed);                              //设置最小速度 1
  Write429Short(IDX_VMIN|MOTOR1, MotorConfig[1].mini_speed);
  Write429Short(IDX_VMIN|MOTOR2, MotorConfig[2].mini_speed);

  Write429Int(IDX_VMAX|MOTOR0, MotorConfig[0].VMax);              //设置速度
  SetAMax(0, MotorConfig[0].AMax);                                //设置加速度
  Write429Int(IDX_VMAX|MOTOR1, MotorConfig[1].VMax);
  SetAMax(1, MotorConfig[1].AMax);
  Write429Int(IDX_VMAX|MOTOR2, MotorConfig[2].VMax);
  SetAMax(2, MotorConfig[2].AMax);
}
/*******************************************************************************
* 函 数 名     : Read429VERSION
* 函数功能		: 读取429版本信息 
* 输    入     : 无
* 输    出     : 无
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
  //参数存储到EEPROM
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



