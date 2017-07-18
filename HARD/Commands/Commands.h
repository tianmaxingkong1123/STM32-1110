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
#ifndef __Commands_H
#define __Commands_H

#include "sys.h"
#include "globals.h"
#include "CAN.h"

#define TCS_IDLE  0 
#define TCS_CAN   1
#define TCS_CAN_ERROR 2       //!< last command from RS485 had bad check sum



//Reply format
#define RF_STANDARD 0               //!< use standard TMCL reply
#define RF_SPECIAL 1                //!< use special reply




//TMCL status codes
#define REPLY_OK 100                //!< command successfully executed
#define REPLY_CHKERR 1              //!< checksum error
#define REPLY_INVALID_CMD 2         //!< command not supported
#define REPLY_WRONG_TYPE 3          //!< wrong type code
#define REPLY_INVALID_VALUE 4       //!< wrong value
#define REPLY_EEPROM_LOCKED 5       //!< EEPROM is locked
#define REPLY_CMD_NOT_AVAILABLE 6   //!< command not available due to current state
#define REPLY_CMD_LOAD_ERROR 7      //!< error when storing command to EEPROM
#define REPLY_WRITE_PROTECTED 8     //!< EEPROM is write protected
#define REPLY_MAX_EXCEEDED 9        //!< maximum number of commands in EEPROM exceeded



//Supported TMCL commands
#define TMCL_ROR   1           //!< ROR  command opcode
#define TMCL_ROL   2           //!< ROL  command opcode
#define TMCL_MST   3           //!< MST  command opcode
#define TMCL_MVP   4           //!< MVP  command opcode
#define TMCL_SAP   5           //!< SAP  command opcode
#define TMCL_GAP   6           //!< GAP  command opcode
#define TMC_STAP   7           //!< STAP command opcode
#define TMC_RSAP   8           //恢复轴参数
#define TMC_SGP    9           //设置全局参数
#define TMC_GGP    10          //得到全局参数

#define TMC_RSGP   12          //恢复全局参数
#define TMC_RFS    13          //自动寻位
#define TMC_SIO    14          //设置IO 
#define TMC_GIO    15          //得到IO 



#define TMCL_GetVersion 136    //!< GetVersion command opcode
#define TMCL_Boot 0xf2         //!< Boot command opcode
#define TMCL_SoftwareReset 0xff  //!< software reset command opcode


typedef struct
{
  u8 Opcode;             //!< command opcode
  u8 Type;               //!< type parameter
  u8 Motor;              //!< motor/bank parameter
  union
  {
    long Int32;          //!< value parameter as 32 bit integer
    u8 Byte[4];          //!< value parameter as 4 bytes
  } Value;               //!< value parameter
} TMCLCommand;


//! TMCL reply
typedef struct
{
  u8 Status;              //!< status code
  u8 Opcode;              //!< opcode of executed command
  union
  {
    long Int32;           //!< reply value as 32 bit integer
    u8 Byte[4];           //!< reply value as 4 bytes
  } Value;                //!< value parameter
} TMCLReply;


extern u8  CANCmd[9];                       //!< CAN command buffer
extern u8  CANCount;                        //!< CANcommnd byte counter
extern u8  TMCLCommandState;                //!< State of the interpreter
extern u8  TMCLReplyFormat;                  //!< format of next reply (RF_NORMAL or RF_SPECIAL)
extern u8  SpecialReply[9];                  //!< buffer for special replies
extern u8  SpeedChangedFlag[NUM_Motor];      //!< TRUE when motor max. speed has been changed by ROL/ROR command
extern u8  ResetRequested;                   //!< TRUE after executing the software reset command
extern  TModuleConfig ModuleConfig;
extern u8  CAN_ReceiveFlag;
extern u8  CAN_receive_ADDR;
extern u8  RFS_FLAG[NUM_Motor];

extern TMCLCommand ActualCommand; 
extern TMCLReply ActualReply;              //!< Reply of last executed TMCL command



void PromcessCommands(void);
void ExecuteActualCommand(void);
void SetAxisParameter(void);
void GetAxisParameter(void);
void Store_AxisParameter(void);
void Restore_AxisParameter(void);

void Set_GlobalParameter(void);
void Get_GlobalParameter(void);
void Restore_GlobalParameter(void);
void Reference_search(void) ;
void SIO_Output(void);
void  GIO_INput(void);

#endif

