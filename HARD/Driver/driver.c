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

#include "driver.h"
#include "Globals.h"
#include "TMC429.h"
#include "Commands.h"


/*******************************************************************************
* �� �� ��     : RotateRight
* ��������		: ������ת����
* ��    ��     : ��ַ
* ��    ��     : 0 ����1 ����
*******************************************************************************/


void RotateRight(void)   
{
  // ActualCommand.Motor=0;                 //����
  // ActualCommand.Value.Int32=200;
   
 if(ActualCommand.Motor<NUM_Motor)
  {
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;

}
/*******************************************************************************
* �� �� ��     : RotateLeft
* ��������		: ������ת����
* ��    ��     : ��ַ
* ��    ��     : 0 ����1 ����
*******************************************************************************/
void RotateLeft(void)
{
      
  if(ActualCommand.Motor<NUM_Motor)
  {
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), -ActualCommand.Value.Int32);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}
/*******************************************************************************
* �� �� ��     : MotorStop
* ��������		: ֹͣ
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/

void MotorStop(void)
{
   
 if(ActualCommand.Motor<NUM_Motor)
  {
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);           //���޸���ɡ�����
    Write429Zero(IDX_VTARGET|(ActualCommand.Motor<<5));
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}
/*******************************************************************************
* �� �� ��     : MotorStop
* ��������		: ֹͣ
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void MoveToPosition(void)
{
  if(ActualCommand.Motor<NUM_Motor)
  {
    if(ActualCommand.Type==MVP_ABS || ActualCommand.Type==MVP_REL)
    {
      if(SpeedChangedFlag[ActualCommand.Motor])
      {
        Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);    //д����ٶ�
        SetAMax(ActualCommand.Motor, MotorConfig[ActualCommand.Motor].AMax);                        //���ü��ٶ�
        SpeedChangedFlag[ActualCommand.Motor]=FALSE;                                                //������ı��ٶ�
      }
       Set429RampMode(ActualCommand.Motor, RM_RAMP);                                                //����ģʽ
      if(ActualCommand.Type==MVP_ABS)
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);
      else
        Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5), 
                     ActualCommand.Value.Int32+Read429Int(IDX_XACTUAL|(ActualCommand.Motor<<5)));
      
       ActualReply.Value.Int32=Read429Int(IDX_XTARGET|(ActualCommand.Motor<<5));                    //�ظ�Ŀ��λ��
    }
    
    
    
    else ActualReply.Status=REPLY_WRONG_TYPE;
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}
