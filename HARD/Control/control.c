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
* 函 数 名     : SystemControl()
* 函数功能		: 系统控制，控制262相关电流的设置，锁存，启动
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void SystemControl(void)            
{

 int Velocity;              //当前速度
 u32 i;
 u8  j;
   
   if(j>=50)
      {
        RFS_Control();j=0;               //自动寻位控制，可能存在BUG
      }
   
   //ActualAxis=0;   
  if(!StandbyTimerInitFlag)                                  
   {
      for(i=0; i<NUM_Motor; i++) StandbyTimer[i]=INT_MAX;               //初始话时间标志
      StandbyTimerInitFlag=TRUE;
   }

  if(abs(Tick_Counter-Delay)>10)                                         //10ms-Tasks
   {
      j++;
     Velocity=Read429Short(MOTOR_NUMBER(ActualAxis)<<5|IDX_VACTUAL);     //当前速度

    //Stromregelung (Umschaltung Run current / Stand by current)
    if(Velocity==0 && Read429Short(MOTOR_NUMBER(ActualAxis)<<5|IDX_VTARGET)==0)   //速度为0&&目标速度为0
    {
      if(abs(Tick_Counter-StandbyTimer[ActualAxis])>MotorConfig[ActualAxis].SettingDelay*3)  //锁存延迟时间1 s
      {         
        // i=MotorConfig[ActualAxis].IStandby;
        SetMotorCurrent(ActualAxis, MotorConfig[ActualAxis].IStandby);         //电流锁存，默认
        StandbyFlag[ActualAxis]=TRUE;                                          //锁存标志
           
      }
    }
    else
    {
      //Zus zliche Stromabsenkung bei niedriger Geschwindigkeit
      //(kann abgeschaltet werden indem man ThresholdSpeed auf 0 setzt).
      if(abs(Velocity)<CoolStepConfig[ActualAxis].ThresholdSpeed)                        //速度小于阈值速度
        SetMotorCurrent(ActualAxis, CoolStepConfig[ActualAxis].SlowRunCurrent);          //设置电流 慢速
      else
        SetMotorCurrent(ActualAxis, MotorConfig[ActualAxis].IRun);                       //设置运行电流

        StandbyTimer[ActualAxis]=Tick_Counter;                                           //提取系统时间
        StandbyFlag[ActualAxis]=FALSE;                                                   //清锁存标志
    }

      //Geschwindigkeitsabh鋘giges Ein- und Ausschalten von CoolStep
      //(nicht wenn ThresholdSpeed==0)
      if(abs(Velocity)<CoolStepConfig[ActualAxis].ThresholdSpeed)                        //速度小于阈值速度
      {
        //CoolStep ausschalten wenn nicht schon ausgeschaltet
        if(Get262SmartEnergyStallLevelMin(WHICH_262(ActualAxis))==CoolStepConfig[ActualAxis].HysteresisStart)
        {
          Set262SmartEnergyStallLevelMin(WHICH_262(ActualAxis), 0);                     //设置 ，衰减，待验证
        }
      }
      else
      {
        //CoolStep einschalten wenn nicht bereits eingeschaltet (auf den mit SAP 172 eingestellten Wert
        //zur點ksetzen - wenn dieser 0 ist bleibt CoolStep natrlich ausgeschaltet).
        if(Get262SmartEnergyStallLevelMin(WHICH_262(ActualAxis))!=CoolStepConfig[ActualAxis].HysteresisStart)
        {
          Set262SmartEnergyStallLevelMin(WHICH_262(ActualAxis), CoolStepConfig[ActualAxis].HysteresisStart);
        }
      }

    //berwachung der Treiber
    Read262State(WHICH_262(ActualAxis), NULL, NULL, &StallLevel[ActualAxis], 
                              &SmartEnergy[ActualAxis], &DriverFlags[ActualAxis]);

    //Stop bei Stall
    if(MotorConfig[ActualAxis].StallVMin>0&&GetStallState(ActualAxis))          //失速
    {
      if(abs(Velocity)>MotorConfig[ActualAxis].StallVMin)                       //失速，进行电机停止
      {
        HardStop(ActualAxis);
        StallFlag[ActualAxis]=TRUE;                                             //锁存标志
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

    //N鋍hste Achse
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
   if(RFS_FLAG[i]==1)    //开始寻位
     {   
         if(((Read429Status()>>(i*2+1))&0x01)&&flag[i]==0)               //判断是否到达
         {
           delay[i]=Tick_Counter;
           flag[i]=1;
           delay[i]=Tick_Counter;
           // ActualReply.Value.Byte[0]=1;                 //回复限位开关状态   

           // Set429RampMode(i, RM_RAMP);
            Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), 0);   //寻位速度
            Write429Int(IDX_XACTUAL|(i<<5),0);             //写当前位置   
            Write429Int(IDX_XTARGET|(i<<5),0);             //写目标位置      
            Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].RFS_Speed/2);           
         }             
         
       if(flag[i]==1&&((Read429Status()>>(i*2+1))&0x01)==0&&((abs(Tick_Counter-delay[i]))>=100))
        {
         delay[i]=Tick_Counter;
         Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), -MotorConfig[ActualCommand.Motor].RFS_Speed/2);   //寻位速度               
         flag[i]=3;      
        }
       if(flag[i]==3&&((Read429Status()>>(i*2+1))&0x01)==1&&((abs(Tick_Counter-delay[i]))>=100))
        {     
         //Set429RampMode(i, RM_RAMP);
         Write429Int(IDX_XACTUAL|(i<<5),0);             //写当前位置   
         Write429Int(IDX_XTARGET|(i<<5),0);             //写目标位置  
         Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), 0);   //寻位速度  
         flag[i]=0;
         RFS_FLAG[i]=0;        
        }
     }
  }
}



#if   0                         
         }  
   //如果不启动再次寻位功能，请在上述添加         
        if(((abs(Tick_Counter-delay[i]))>=100)&&(flag[i]==1))
          {        
           //Set429RampMode(i, RM_RAMP);
           //Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);    //写最大速度
           Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].RFS_Speed*2);    //最大速度  
           Write429Int(IDX_XTARGET|(i<<5),1000);
           flag[i]=0;
           RFS_FLAG[i]=2;                                 //第一次寻位完成
          }
      } 
     

    if(RFS_FLAG[i]==2)                                     //第二次寻位寻位
        {              
           if(Read429Int(IDX_XACTUAL|(i<<5))==1000)              
           {
              flag[i]=3;
              SpeedChangedFlag[ActualCommand.Motor]=TRUE;
              Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
              Write429Short(IDX_VMAX|(i<<5), 2047);                //加速度                 
              Write429Short(IDX_VTARGET|(i<<5), -MotorConfig[ActualCommand.Motor].RFS_Speed/2);              //寻位速度                      
            }
           if(((Read429Status()>>(i*2+1))&0x01)&&(flag[i]==3))             //判断是否到达
           {
              // ActualReply.Value.Byte[0]=1;              //回复限位开关状态   

               Set429RampMode(i, RM_RAMP);
               Write429Int(IDX_XACTUAL|(i<<5),0);           //写当前位置   
               Write429Int(IDX_XTARGET|(i<<5),0);           //写0点            
               RFS_FLAG[i]=0;                               //寻位完成
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







