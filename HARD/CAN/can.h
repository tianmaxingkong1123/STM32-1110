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

#ifndef  __CAN_H
#define  __CAN_H

#include "sys.h"


typedef struct
{
  u8 CAN_Bitrate;           //!< CAN baud rate (0..7, 0=9600bps)
  u8 CAN_ModuleAddress;     //!< CAN TMCL module address
  u8 CAN_ReplyAddress;      //!< CAN TMCL reply address
} TModuleConfig;



//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.								    
										 							 				    
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��
 
u8 Can_Send_Msg(u8* msg,u8 len);						//��������

u8 Can_Receive_Msg(u8 *buf);							//��������
void CAN_Init_ID(void);
void Can_usart(u8 id,u8 buf0,u8 buf1,u8 buf2,u32 buf3);

#endif

