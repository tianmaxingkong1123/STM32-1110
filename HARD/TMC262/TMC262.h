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

#ifndef __TMC262_H
#define __TMC262_H

#include "sys.h"

#define NULL 0                     //定义空指针

#define TMC262_RB_MSTEP        0
#define TMC262_RB_STALL_GUARD  1
#define TMC262_RB_SMART_ENERGY 2
#define TMC262_RB_ENCODER      3

#define ENABLE_DRIVERS  PBout(6)

void InitMotorDrivers(void);
void SetMotorCurrent(u8 Motor, u8 Current);
void Enable262(u8 Which262);
void Disable262(u8 Which262);
u8  Get262SmartEnergyStallLevelMin(u8 Which262);
void Set262SmartEnergyStallLevelMin(u8 Which262, u8 StallLevelMin);
void Read262State(u8 Which262, u8 *Phases, u8 *MStep, u32 *StallGuard, u8 *SmartEnergy, u8 *Flags);

void Set262StallGuardCurrentScale(u8 Which262, u8 CurrentScale) ;      //电流大小
void WriteStepDirConfig(u8 Which262);
void Set262_EEPROM(void);
#endif

