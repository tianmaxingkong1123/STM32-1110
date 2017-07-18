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

#include "Globals.h"


/****************429 ������ؽṹ��***********************/
 TMotorConfig MotorConfig[NUM_Motor]=
{{
  100,    //!< VMax
  50,     //!< AMax
  3,      //!< Pulsediv
  7,      //!< Rampdiv
  80,     //!< IRun  255
  20,     //!< IStandby   32
  0,      //!< StallVMin
  0,      //!< FreewheelingDelay
  200,    //!< SettingDelay
 
  2,      //��С�ٶ�
  100,    //Ѱλ�ٶ�   
  1,      //����λ���ر�
  1,      //����λ���ر�
  128,
 },
{
  500,    //!< VMax
  50,     //!< AMax
  3,      //!< Pulsediv
  7,      //!< Rampdiv
  100,    //!< IRun  255
  20,     //!< IStandby   32
  0,      //!< StallVMin
  0,      //!< FreewheelingDelay
  200,    //!< SettingDelay
   
  2,      //��С�ٶ�
  100,    //Ѱλ�ٶ�   
  1,      //����λ
  1,      //����λ
  128,
 },
{
  500,    //!< VMax
  50,     //!< AMax
  3,      //!< Pulsediv
  7,      //!< Rampdiv
  100,    //!< IRun  255
  20,     //!< IStandby   32
  1,      //!< StallVMin
  1,      //!< FreewheelingDelay
  200,    //!< SettingDelay
   
  2,     //��С�ٶ�
  100,   //Ѱλ�ٶ�   
  1,     //����λ
  1,     //����λ
  128,
 },
};

u8  SmartEnergy[NUM_Motor];               //!< actual smartEnergy values      
u8  StallFlag[NUM_Motor];                 //!< actual stallGuard flag states   
u32 StallLevel[NUM_Motor];                //!< actual stallGuard load values
u8  DriverFlags[NUM_Motor];               //!< actual driver error flags
u8  MotorDisable[NUM_Motor];              //!< actual motor disable flags
u8  StandbyFlag[NUM_Motor];               //!< standby current flags
u8  FreewheelingActive[NUM_Motor];        //!< freewheeling flags
u8  SlowRunCurrent[NUM_Motor];            //!< slow run current



/************************262*�����ṹ��********************/
TStepDirConfig  StepDirConfig[NUM_Motor]=
{
  {
     0,   //�����ֵ
     0,   //����ѡ��
     4    //ϸ��
  },
  {
     0,   //�����ֵ
     0,   //����ѡ��
     3    //ϸ��
  },
  {
     0,   //�����ֵ
     0,   //����ѡ��
     3    //ϸ��
  },   
};

TCoolStepConfig     CoolStepConfig[NUM_Motor]=
{
   {
      0,
      0,
      2               //����˥���ķ�����ֵ��������ֵ�Ż����˥��
   },
   {
      0,
      0,
      0
   },
   {
      0,
      0,
      0
   },

};

//Global variables of this module
//TStepDirConfig      StepDirConfig[NUM_Motor];         //!< Software copies of the DRVCTRL registers of all TMC26x ICs
 TChopperConfig      ChopperConfig[NUM_Motor];          //!< Software copies of the CHOPCONF registers of all TMC26x ICs   ****
 TSmartEnergyControl SmartEnergyControl[NUM_Motor];     //!< Software copies of the SMARTEN registers of all TMC26x ICs    ****
 TStallGuardConfig   StallGuardConfig[NUM_Motor];       //!< Software copies of the SGSCONF registers of all TMC26x ICs    ****
 TDriverConfig       DriverConfig[NUM_Motor];           //!< Software copies of the DRVCONF registers of all TMC26x ICs    ****
 
 //TCoolStepConfig     CoolStepConfig[NUM_Motor];         //!< coolStep configuration

 TReadBackDatagram   ReadBackDatagram[NUM_Motor];       //!< Next datagram type used for reading the actual state of the TMC26x
 
 
u32 SPIReadInt;                                         //!< SPI read buffer
u32 SPIWriteInt;                                        //!< SPI write buffer

