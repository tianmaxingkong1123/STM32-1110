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

#ifndef __TMC262_H
#define __TMC262_H

#include "sys.h"

#define NULL 0                     //�����ָ��

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

void Set262StallGuardCurrentScale(u8 Which262, u8 CurrentScale) ;      //������С
void WriteStepDirConfig(u8 Which262);
void Set262_EEPROM(void);
#endif

