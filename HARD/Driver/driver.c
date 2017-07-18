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

#include "driver.h"
#include "Globals.h"
#include "TMC429.h"
#include "Commands.h"


/*******************************************************************************
* 函 数 名     : RotateRight
* 函数功能		: 向右旋转命令
* 输    入     : 地址
* 输    出     : 0 错误，1 正常
*******************************************************************************/


void RotateRight(void)   
{
  // ActualCommand.Motor=0;                 //测试
  // ActualCommand.Value.Int32=200;
   
 if(ActualCommand.Motor<NUM_Motor)
  {
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;

}
/*******************************************************************************
* 函 数 名     : RotateLeft
* 函数功能		: 向右旋转命令
* 输    入     : 地址
* 输    出     : 0 错误，1 正常
*******************************************************************************/
void RotateLeft(void)
{
      
  if(ActualCommand.Motor<NUM_Motor)
  {
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), -ActualCommand.Value.Int32);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}
/*******************************************************************************
* 函 数 名     : MotorStop
* 函数功能		: 停止
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/

void MotorStop(void)
{
   
 if(ActualCommand.Motor<NUM_Motor)
  {
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);           //已修改完成。。。
    Write429Zero(IDX_VTARGET|(ActualCommand.Motor<<5));
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}
/*******************************************************************************
* 函 数 名     : MotorStop
* 函数功能		: 停止
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void MoveToPosition(void)
{
  if(ActualCommand.Motor<NUM_Motor)
  {
    if(ActualCommand.Type==MVP_ABS || ActualCommand.Type==MVP_REL)
    {
      if(SpeedChangedFlag[ActualCommand.Motor])
      {
        Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);    //写最大速度
        SetAMax(ActualCommand.Motor, MotorConfig[ActualCommand.Motor].AMax);                        //设置加速度
        SpeedChangedFlag[ActualCommand.Motor]=FALSE;                                                //不允许改变速度
      }
       Set429RampMode(ActualCommand.Motor, RM_RAMP);                                                //设置模式
      if(ActualCommand.Type==MVP_ABS)
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
      else
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), 
                     ActualCommand.Value.Int32+Read429Int(IDX_XACTUAL|(ActualCommand.Motor<<5)));
      
       ActualReply.Value.Int32=Read429Int(IDX_XTARGET|(ActualCommand.Motor<<5));                    //回复目标位置
    }
    
    
    
    else ActualReply.Status=REPLY_WRONG_TYPE;
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}
