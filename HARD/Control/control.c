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
#include <stdlib.h>
#include "control.h"
#include "tmc429.h"
#include "TMC262.h"
#include "Globals.h"
#include "GPIO.h"
#include "Commands.h"

u8  ActualAxis;                      //!< monitored axis
u32 Delay;                            //!< Delay timer
u8  StandbyTimerInitFlag;            //!< TRUE after the standby timers have been intialized
u32 StandbyTimer[NUM_Motor];         //!< Standby timers
u32 FreewheelingTimer[NUM_Motor];    //!< Freewheeling timers


void RFS_Control(void);
/*******************************************************************************
* �� �� ��     : SystemControl()
* ��������		: ϵͳ���ƣ�����262��ص��������ã����棬����
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void SystemControl(void)            
{

 int Velocity;              //��ǰ�ٶ�
 u32 i;
 u8  j;
   
   if(j>=50)
      {
        RFS_Control();j=0;               //�Զ�Ѱλ���ƣ����ܴ���BUG
      }
   
   //ActualAxis=0;   
  if(!StandbyTimerInitFlag)                                  
   {
      for(i=0; i<NUM_Motor; i++) StandbyTimer[i]=INT_MAX;               //��ʼ��ʱ���־
      StandbyTimerInitFlag=TRUE;
   }

  if(abs(Tick_Counter-Delay)>10)                                         //10ms-Tasks
   {
      j++;
     Velocity=Read429Short(MOTOR_NUMBER(ActualAxis)<<5|IDX_VACTUAL);     //��ǰ�ٶ�

    //Stromregelung (Umschaltung Run current / Stand by current)
    if(Velocity==0 && Read429Short(MOTOR_NUMBER(ActualAxis)<<5|IDX_VTARGET)==0)   //�ٶ�Ϊ0&&Ŀ���ٶ�Ϊ0
    {
      if(abs(Tick_Counter-StandbyTimer[ActualAxis])>MotorConfig[ActualAxis].SettingDelay*3)  //�����ӳ�ʱ��1 s
      {         
        // i=MotorConfig[ActualAxis].IStandby;
        SetMotorCurrent(ActualAxis, MotorConfig[ActualAxis].IStandby);         //�������棬Ĭ��
        StandbyFlag[ActualAxis]=TRUE;                                          //�����־
           
      }
    }
    else
    {
      //Zus zliche Stromabsenkung bei niedriger Geschwindigkeit
      //(kann abgeschaltet werden indem man ThresholdSpeed auf 0 setzt).
      if(abs(Velocity)<CoolStepConfig[ActualAxis].ThresholdSpeed)                        //�ٶ�С����ֵ�ٶ�
        SetMotorCurrent(ActualAxis, CoolStepConfig[ActualAxis].SlowRunCurrent);          //���õ��� ����
      else
        SetMotorCurrent(ActualAxis, MotorConfig[ActualAxis].IRun);                       //�������е���

        StandbyTimer[ActualAxis]=Tick_Counter;                                           //��ȡϵͳʱ��
        StandbyFlag[ActualAxis]=FALSE;                                                   //�������־
    }

      //Geschwindigkeitsabh�ngiges Ein- und Ausschalten von CoolStep
      //(nicht wenn ThresholdSpeed==0)
      if(abs(Velocity)<CoolStepConfig[ActualAxis].ThresholdSpeed)                        //�ٶ�С����ֵ�ٶ�
      {
        //CoolStep ausschalten wenn nicht schon ausgeschaltet
        if(Get262SmartEnergyStallLevelMin(WHICH_262(ActualAxis))==CoolStepConfig[ActualAxis].HysteresisStart)
        {
          Set262SmartEnergyStallLevelMin(WHICH_262(ActualAxis), 0);                     //���� ��˥��������֤
        }
      }
      else
      {
        //CoolStep einschalten wenn nicht bereits eingeschaltet (auf den mit SAP 172 eingestellten Wert
        //zur�cksetzen - wenn dieser 0 ist bleibt CoolStep natrlich ausgeschaltet).
        if(Get262SmartEnergyStallLevelMin(WHICH_262(ActualAxis))!=CoolStepConfig[ActualAxis].HysteresisStart)
        {
          Set262SmartEnergyStallLevelMin(WHICH_262(ActualAxis), CoolStepConfig[ActualAxis].HysteresisStart);
        }
      }

    //berwachung der Treiber
    Read262State(WHICH_262(ActualAxis), NULL, NULL, &StallLevel[ActualAxis], 
                              &SmartEnergy[ActualAxis], &DriverFlags[ActualAxis]);

    //Stop bei Stall
    if(MotorConfig[ActualAxis].StallVMin>0&&GetStallState(ActualAxis))          //ʧ��
    {
      if(abs(Velocity)>MotorConfig[ActualAxis].StallVMin)                       //ʧ�٣����е��ֹͣ
      {
        HardStop(ActualAxis);
        StallFlag[ActualAxis]=TRUE;                                             //�����־
      }
    }

    //Freewheeling Delay: Motor stromlos schalten nach angebener Stillstandzeit
    if((Velocity==0 && Read429Short(MOTOR_NUMBER(ActualAxis)<<5|IDX_VTARGET)==0) || MotorDisable[ActualAxis])
    {
      if(MotorConfig[ActualAxis].FreewheelingDelay>0 || MotorDisable[ActualAxis])
      {
        if(abs(Tick_Counter-FreewheelingTimer[ActualAxis])>MotorConfig[ActualAxis].FreewheelingDelay*10)
        {
          if(!FreewheelingActive[ActualAxis])
          {
            Disable262(WHICH_262(ActualAxis));    
            FreewheelingActive[ActualAxis]=TRUE;
          }
        }
      }
    }
    else FreewheelingTimer[ActualAxis]=Tick_Counter;

    //N�chste Achse
    ActualAxis++;
    if(ActualAxis>=NUM_Motor)  ActualAxis=0;

    Delay=Tick_Counter;
  }
}



void RFS_Control(void)
   
{
   u8 i;
  static  u32 delay[3];
  static  u8  flag[3]={0};
   for(i=0;i<=2;i++)
   {
   if(RFS_FLAG[i]==1)    //��ʼѰλ
     {   
         if(((Read429Status()>>(i*2+1))&0x01)&&flag[i]==0)               //�ж��Ƿ񵽴�
         {
           delay[i]=Tick_Counter;
           flag[i]=1;
           delay[i]=Tick_Counter;
           // ActualReply.Value.Byte[0]=1;                 //�ظ���λ����״̬   

           // Set429RampMode(i, RM_RAMP);
            Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), 0);   //Ѱλ�ٶ�
            Write429Int(IDX_XACTUAL|(i<<5),0);             //д��ǰλ��   
            Write429Int(IDX_XTARGET|(i<<5),0);             //дĿ��λ��      
            Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].RFS_Speed/2);           
         }             
         
       if(flag[i]==1&&((Read429Status()>>(i*2+1))&0x01)==0&&((abs(Tick_Counter-delay[i]))>=100))
        {
         delay[i]=Tick_Counter;
         Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), -MotorConfig[ActualCommand.Motor].RFS_Speed/2);   //Ѱλ�ٶ�               
         flag[i]=3;      
        }
       if(flag[i]==3&&((Read429Status()>>(i*2+1))&0x01)==1&&((abs(Tick_Counter-delay[i]))>=100))
        {     
         //Set429RampMode(i, RM_RAMP);
         Write429Int(IDX_XACTUAL|(i<<5),0);             //д��ǰλ��   
         Write429Int(IDX_XTARGET|(i<<5),0);             //дĿ��λ��  
         Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), 0);   //Ѱλ�ٶ�  
         flag[i]=0;
         RFS_FLAG[i]=0;        
        }
     }
  }
}



#if   0                         
         }  
   //����������ٴ�Ѱλ���ܣ������������         
        if(((abs(Tick_Counter-delay[i]))>=100)&&(flag[i]==1))
          {        
           //Set429RampMode(i, RM_RAMP);
           //Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);    //д����ٶ�
           Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].RFS_Speed*2);    //����ٶ�  
           Write429Int(IDX_XTARGET|(i<<5),1000);
           flag[i]=0;
           RFS_FLAG[i]=2;                                 //��һ��Ѱλ���
          }
      } 
     

    if(RFS_FLAG[i]==2)                                     //�ڶ���ѰλѰλ
        {              
           if(Read429Int(IDX_XACTUAL|(i<<5))==1000)              
           {
              flag[i]=3;
              SpeedChangedFlag[ActualCommand.Motor]=TRUE;
              Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
              Write429Short(IDX_VMAX|(i<<5), 2047);                //���ٶ�                 
              Write429Short(IDX_VTARGET|(i<<5), -MotorConfig[ActualCommand.Motor].RFS_Speed/2);              //Ѱλ�ٶ�                      
            }
           if(((Read429Status()>>(i*2+1))&0x01)&&(flag[i]==3))             //�ж��Ƿ񵽴�
           {
              // ActualReply.Value.Byte[0]=1;              //�ظ���λ����״̬   

               Set429RampMode(i, RM_RAMP);
               Write429Int(IDX_XACTUAL|(i<<5),0);           //д��ǰλ��   
               Write429Int(IDX_XTARGET|(i<<5),0);           //д0��            
               RFS_FLAG[i]=0;                               //Ѱλ���
               flag[i]=0;
           }        
         } 
        
         
         

   }
//#else
   
   flag[i]=0;
      }
    }
  }
   
  
}
 #endif







