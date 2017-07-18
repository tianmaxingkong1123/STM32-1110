#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "sys.h"

#define FALSE	0
#define TRUE	1


#define NUM_Motor  3
#define MOTOR_NUMBER(a)  a    //���ѡ��  ��չ
#define WHICH_262(a)     a    

/****************429 ��������洢�ṹ��**********************/

typedef struct
{
  u32 VMax;                   //!< maximum positioning velocity
  u32 AMax;                   //!< maximum acceleration
  u8  PulseDiv;               //!< pulse divisor (TMC429)
  u8  RampDiv;                //!< ramp divisor (TMC429)
  u8  IRun;                   //!< run current (0..255)
  u8  IStandby;               //!< stand-by current(0..255)
  u32 StallVMin;              //!< minimum speed for stallGuard
  u32 FreewheelingDelay;      //!< freewheeling delay time (*10ms)
  u32 SettingDelay;           //!< delay for switching to stand-by current (*10ms)  
   
  u32 mini_speed;             //��С�ٶ�
  u32 RFS_Speed;              //Ѱλ�ٶ� 
  u8  right_limit;            //����λʹ��
  u8  left_limit;             //����λʹ��
  u16 Encoder_pre;

   
} TMotorConfig; 


//! coolStep configuration data
typedef struct
{
  u32 ThresholdSpeed;         //!< coolStep threshold speed
  u8  SlowRunCurrent;         //!< coolStep slow run current
  u8  HysteresisStart;        //!< coolStep hysteresis start
} TCoolStepConfig;


extern  u8  SmartEnergy[NUM_Motor];                //!< actual smartEnergy values
extern  u8  StallFlag[NUM_Motor];                  //!< actual stallGuard flag states
extern  u32 StallLevel[NUM_Motor];                 //!< actual stallGuard load values
extern  u8  DriverFlags[NUM_Motor];                //!< actual driver error flags
extern  u8  MotorDisable[NUM_Motor];               //!< actual motor disable flags
extern  u8  StandbyFlag[NUM_Motor];                //!< standby current flags
extern  u8  FreewheelingActive[NUM_Motor];         //!< freewheeling flags
extern  u8  SlowRunCurrent[NUM_Motor];             //!< slow run current

/**************************262��ز���*************************/
typedef struct          //ϸ�ֵȲ���
{
  u8 Intpol;            //�����ֵ
  u8 DEdge;             //����ѡ��
  u8 MRes;              //ϸ��
} TStepDirConfig;

//! Data structure for software copy of the TMC262 CHOPCONF register
typedef struct                      //ն����ز���
{
  u8 BlankTime;                     //����ʱ��
  u8 ChopperMode;                           
  u8 HysteresisDecay;
  u8 RandomTOff;                    //
  u8 HysteresisEnd;                 //˥��ʱ��
  u8 HysteresisStart;               
  u8 TOff;                          //MOS�ܿ���
  u8 DisableFlag;                   //ʹ��״̬
} TChopperConfig;

//! Data structure for software copy of the TMC262 SMARTEN register
typedef struct                      //��������
{
  u8 SmartIMin;                     //��С
  u8 SmartDownStep;                 //˥���ٶ�
  u8 SmartStallLevelMax;            //������ֵ
  u8 SmartUpStep;                   //����
  u8 SmartStallLevelMin;            //��С
} TSmartEnergyControl;

//! Data structure for software copy of the TMC262 SGSCONF register
typedef struct                              // stallGuard2  TMC262 SGSCONF register
{
  u8 FilterEnable;                          //����ģʽ
  signed char StallGuardThreshold;          //�ٽ�ֵ
  u8 CurrentScale;                          //������С
} TStallGuardConfig;

//! Data structure for software copy of the TMC262 DRVCONF register
typedef struct                            //����ģʽ
{
  u8 SlopeHighSide;                       //�¶Ȳ���v��λ
  u8 SlopeLowSide;                        //��λ             
  u8 ProtectionDisable;                   //�ӵر���
  u8 ProtectionTimer;                     //ʱ��
  u8 StepDirectionDisable;                //sd ģʽ
  u8 VSenseScale;                         //������
  u8 ReadBackSelect;                      //����ģʽ
} TDriverConfig;

//! Possible TMC262 read back modes
typedef enum
{
  RB_CHOPPER,
  RB_DRIVER, 
  RB_SMART_ENERGY, 
  RB_STALL_GUARD, 
  RB_STEP_DIR
} TReadBackDatagram;


//Global variables of this module
extern TStepDirConfig      StepDirConfig[NUM_Motor];         //!< Software copies of the DRVCTRL registers of all TMC26x ICs
extern TChopperConfig      ChopperConfig[NUM_Motor];         //!< Software copies of the CHOPCONF registers of all TMC26x ICs
extern TSmartEnergyControl SmartEnergyControl[NUM_Motor];    //!< Software copies of the SMARTEN registers of all TMC26x ICs
extern TStallGuardConfig   StallGuardConfig[NUM_Motor];      //!< Software copies of the SGSCONF registers of all TMC26x ICs
extern TDriverConfig       DriverConfig[NUM_Motor];          //!< Software copies of the DRVCONF registers of all TMC26x ICs
extern TReadBackDatagram   ReadBackDatagram[NUM_Motor];      //!< Next datagram type used for reading the actual state of the TMC26x
extern u32 SPIReadInt;                                        //!< SPI read buffer
extern u32 SPIWriteInt;                                       //!< SPI write buffer

extern TCoolStepConfig CoolStepConfig[NUM_Motor];  //!< coolStep configuration
extern TMotorConfig MotorConfig[];
//extern TMotorConfig2 MotorConfig2[];
//extern TMotorConfig MotorConfig[NUM_Motor];
#endif

