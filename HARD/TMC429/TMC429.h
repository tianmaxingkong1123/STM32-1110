#ifndef __TMC429_H
#define __TMC429_H

#include "sys.h"


/***************定义各参数***********************/
#define MOTOR0  0
#define MOTOR1  0x20
#define MOTOR2  0X40

/****************429轴参数指令地址********************/
#define IDX_XTARGET           0X00
#define IDX_XACTUAL           0X02
#define IDX_VMIN              0X04
#define IDX_VMAX              0X06
#define IDX_VTARGET           0X08
#define IDX_VACTUAL           0x0A
#define IDX_AMAX              0X0C
#define IDX_AACTUAL           0X0E
#define IDX_AGTAT_ALEAT       0X10
#define IDX_PMUL_PDIV         0x12
#define IDX_REFCONF_RM        0x14
#define IDX_IMASK_IFLAGS      0x16   
#define IDX_PULSEDIV_RAMPDIV  0x18
#define IDX_DX_REFTOLERANCE   0x1A
#define IDX_XLATCHED          0x1C
#define IDX_USTEP_COUNT_429   0x1E 

/****************429全局参数指令地址********************/
#define IDX_LOW_WORD          0x60
#define IDX_HIGH_WORD         0x62
#define IDX_COVER_POS_LEN     0x64
#define IDX_COVER_DATA        0x66
#define IDX_IF_CONFIG_429     0x68
#define IDX_POS_COMP_429      0x6A
#define IDX_POS_COMP_INT_429  0x6C
#define IDX_POWER_DOWN        0X70
#define IDX_TYPE_VERSION_429  0x72
#define IDX_REF_SWITCHES      0x7c
#define IDX_SMGP              0x7e

/****************429读地址****************************/
#define TMC429_READ           0X01

/*****************RAMP_MODE选择**********************/
#define RM_RAMP               0         //RAMP 模式选择          
#define RM_SOFT               1
#define RM_VELOCITY           2
#define RM_HOLD               3

#define NO_REF                0x03       //限位开关设置
#define STOP_REF              0X00    
#define SOFT_REF              0x04

#define NO_LIMIT              0          //
#define HARD_LIMIT            1
#define SOFT_LIMIT            2

#define REFSW_LEFT            0x02
#define REFSW_RIGHT           0x01

#define M0_POS_REACHED        0x01       //读状态位，电机是否执行结束
#define M1_POS_REACHED        0x04
#define M2_POS_REACHED        0x10
#define TMC429_STATUS_CDGW    0x40
#define TMC429_STATUS_INT     0x80   

#define IFLAG_POS_REACHED     0x01        //到达
#define IFLAG_REF_WRONG       0x02
#define IFLAG_REF_MISS        0x04
#define IFLAG_STOP            0x08
#define IFLAG_STOP_LEFT_LOW   0x10
#define IFLAG_STOP_RIGHT_LOW  0x20
#define IFLAG_STOP_LEFT_HIGH  0x40
#define IFLAG_STOP_RIGHT_HIGH 0x80

#define IFCONF_INV_REF        0x0001
#define IFCONF_SDO_INT        0x0002     
#define IFCONF_STEP_HALF      0x0004      //
#define IFCONF_INV_STEP       0x0008
#define IFCONF_INV_DIR        0x0010
#define IFCONF_EN_SD          0x0020      //开模式  STEP/DIR
#define IFCONF_POS_COMP_0     0x0000
#define IFCONF_POS_COMP_1     0x0040
#define IFCONF_POS_COMP_2     0x0080
#define IFCONF_POS_COMP_OFF   0x00C0
#define IFCONF_EN_REFR        0x0100    

void ReadWrite429(u8 *Read, u8 *Write);
void Write429Bytes(u8 Address, u8 *Bytes);
void Write429Datagram(u8 Address, u8 HighByte, u8 MidByte, u8 LowByte);
void Write429Zero(u8 Address);
void Write429Short(u8 Address, int Value);
void Write429Int(u8 Address, int Value);

u8 Read429Status(void);
u8 Read429Bytes(u8 Address, u8 *Bytes);
u8 Read429SingleByte(u8 Address, u8 Index);
int Read429Short(u8 Address);
int Read429Int(u8 Address);

void Set429RampMode(u8 Axis, u8 RampMode);
void Set429SwitchMode(u8 Axis, u8 SwitchMode);
u8 SetAMax(u8 Motor, u32 AMax);
void HardStop(u8 Motor);

void Init429(void);
u32  Read429VERSION(void);
void Set429_EEPROM(void);


#endif
