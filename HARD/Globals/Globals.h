#ifndef __GLOBALS_H
#define __GLOBALS_H

#include "sys.h"

#define FALSE	0
#define TRUE	1


#define NUM_Motor  3
#define MOTOR_NUMBER(a)  a    //电机选择  扩展
#define WHICH_262(a)     a    

/****************429 电机参数存储结构体**********************/

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
   
  u32 mini_speed;             //最小速度
  u32 RFS_Speed;              //寻位速度 
  u8  right_limit;            //右限位使能
  u8  left_limit;             //左限位使能
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

/**************************262相关参数*************************/
typedef struct          //细分等参数
{
  u8 Intpol;            //脉冲插值
  u8 DEdge;             //边沿选择
  u8 MRes;              //细分
} TStepDirConfig;

//! Data structure for software copy of the TMC262 CHOPCONF register
typedef struct                      //斩波相关参数
{
  u8 BlankTime;                     //消退时间
  u8 ChopperMode;                           
  u8 HysteresisDecay;
  u8 RandomTOff;                    //
  u8 HysteresisEnd;                 //衰减时间
  u8 HysteresisStart;               
  u8 TOff;                          //MOS管控制
  u8 DisableFlag;                   //使能状态
} TChopperConfig;

//! Data structure for software copy of the TMC262 SMARTEN register
typedef struct                      //电流设置
{
  u8 SmartIMin;                     //最小
  u8 SmartDownStep;                 //衰减速度
  u8 SmartStallLevelMax;            //补偿阈值
  u8 SmartUpStep;                   //增量
  u8 SmartStallLevelMin;            //最小
} TSmartEnergyControl;

//! Data structure for software copy of the TMC262 SGSCONF register
typedef struct                              // stallGuard2  TMC262 SGSCONF register
{
  u8 FilterEnable;                          //过滤模式
  signed char StallGuardThreshold;          //临界值
  u8 CurrentScale;                          //电流大小
} TStallGuardConfig;

//! Data structure for software copy of the TMC262 DRVCONF register
typedef struct                            //驱动模式
{
  u8 SlopeHighSide;                       //温度补偿v高位
  u8 SlopeLowSide;                        //低位             
  u8 ProtectionDisable;                   //接地保护
  u8 ProtectionTimer;                     //时间
  u8 StepDirectionDisable;                //sd 模式
  u8 VSenseScale;                         //检测电阻
  u8 ReadBackSelect;                      //回送模式
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

