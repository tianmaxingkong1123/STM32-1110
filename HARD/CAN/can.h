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

#ifndef  __CAN_H
#define  __CAN_H

#include "sys.h"


typedef struct
{
  u8 CAN_Bitrate;           //!< CAN baud rate (0..7, 0=9600bps)
  u8 CAN_ModuleAddress;     //!< CAN TMCL module address
  u8 CAN_ReplyAddress;      //!< CAN TMCL reply address
} TModuleConfig;



//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	1		//0,不使能;1,使能.								    
										 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化
 
u8 Can_Send_Msg(u8* msg,u8 len);						//发送数据

u8 Can_Receive_Msg(u8 *buf);							//接收数据
void CAN_Init_ID(void);
void Can_usart(u8 id,u8 buf0,u8 buf1,u8 buf2,u32 buf3);

#endif

