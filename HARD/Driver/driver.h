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

#ifndef __driver_H
#define __driver_H

#include "sys.h"
#include "Commands.h"

//�ƶ�ָ���
#define MVP_ABS   0            //!< absolute movement (with MVP command)
#define MVP_REL   1            //!< relative movement (with MVP command)
#define MVP_COORD 2            //!< coordinate movement (with MVO command)

extern TMCLCommand ActualCommand; 
extern TMCLReply   ActualReply;  
void RotateRight(void) ;
void RotateLeft(void) ;
void MotorStop(void)  ;
void MoveToPosition(void);


#endif   


