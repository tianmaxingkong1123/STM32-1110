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

#ifndef __driver_H
#define __driver_H

#include "sys.h"
#include "Commands.h"

//移动指令定义
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


