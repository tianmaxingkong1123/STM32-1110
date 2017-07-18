/**********************************************************************
* Copyright (c) 2002-2017 山东博科生物产业有限公司 All rights reserved.
*
* 文件名称： Commansds.c
* 描    述： 指令处理文件，包含各指令的解析，恢复，以及动作命令
*
*
* 修改记录：
*
* V1.0，2006.12.22，乔彬
**********************************************************************/
#include "Commands.h"
#include "Globals.h"
#include "CAN.h"
#include "driver.h"
#include "TMC429.h"
#include "TMC262.h"
#include "AT25128B.h"
#include "GPIO.h"
#include "Encoder.h"

u8  CANCmd[9];                        //!< CAN command buffer
u8  CANCount;                         //!< CANcommnd byte counter
u8  TMCLCommandState;                 //!< State of the interpreter
u8  TMCLReplyFormat;                  //!< format of next reply (RF_NORMAL or RF_SPECIAL)
u8  SpeedChangedFlag[NUM_Motor];      //!< TRUE when motor max. speed has been changed by ROL/ROR command
u8  ResetRequested;                   //!< TRUE after executing the software reset command
u8  CAN_ReceiveFlag;
u8  CAN_receive_ADDR;
u8  RFS_FLAG[NUM_Motor];
//u8  SpecialReply[9];                  //!< buffer for special replies

TMCLCommand ActualCommand; 
TMCLReply   ActualReply;              //!< Reply of last executed TMCL command


TModuleConfig ModuleConfig=
{
  8,      //!< can bitrate (0=9600)    100Kbs
  10,      //!< can address
  3,      //!< can reply address  
};

/*******************************************************************************
* 函 数 名     : PromcessCommands
* 函数功能		: 指令处理回复
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void  PromcessCommands(void)
{
 
    u8 send_buf[8];
    u8 i;
    u8 Checksum;
   
   //发送应答指令
   
   if(TMCLCommandState==TCS_CAN)  //via UART
  {
    if(TMCLReplyFormat==RF_STANDARD)             //正常格式回复命令
    {
       /*
       Checksum=ModuleConfig.SerialHostAddress+ModuleConfig.SerialModuleAddress+
               ActualReply.Status+ActualReply.Opcode+
               ActualReply.Value.Byte[3]+
               ActualReply.Value.Byte[2]+
               ActualReply.Value.Byte[1]+
               ActualReply.Value.Byte[0];
      */
       //Can_Send_Msg((u8*)&ModuleConfig,8);//发送8个字节           
         
       //senf_buf[0]=ModuleConfig.SerialHostAddress;
       send_buf[0]=ModuleConfig.CAN_ModuleAddress; 
       send_buf[1]=ActualReply.Status;             //正常的话为100
       send_buf[2]=ActualReply.Opcode;
       send_buf[3]=ActualReply.Value.Byte[3];
       send_buf[4]=ActualReply.Value.Byte[2];
       send_buf[5]=ActualReply.Value.Byte[1];
       send_buf[6]=ActualReply.Value.Byte[0];
       send_buf[7]=0;                             //Checksum        
 
       Can_Send_Msg(send_buf,8);
    }
    else if(TMCLReplyFormat==RF_SPECIAL)           //特殊格式回复命令
    {
      for(i=0; i<9; i++)
      {
      //  WriteRS485(SpecialReply[i]);             //
      }
    }
  }
  
  else if(TMCLCommandState==TCS_CAN_ERROR)   //指令错误回复
  {
       ActualReply.Opcode=0;
       ActualReply.Status=REPLY_CHKERR;     //状态码为1  显示错误
       ActualReply.Value.Int32=0;
     
    //校验计算
   
       send_buf[0]=ModuleConfig.CAN_ModuleAddress; 
       send_buf[1]=ActualReply.Status;
       send_buf[2]=ActualReply.Opcode;
       send_buf[3]=ActualReply.Value.Byte[3];
       send_buf[4]=ActualReply.Value.Byte[2];
       send_buf[5]=ActualReply.Value.Byte[1];
       send_buf[6]=ActualReply.Value.Byte[0];
       send_buf[7]=0;                              //Checksum     
   }
  
     //Reset state (answer has been sent now)
     TMCLCommandState=TCS_IDLE;                   //清状态
     TMCLReplyFormat=RF_STANDARD;                 //正常回复           


     if(ResetRequested) Sys_Soft_Reset();        //系统重启

//**Try to get a new command**
  if(CAN_ReceiveFlag==1)     //Get data from UART
  {
     //此处是否需要加超时报错等的处理，还需要进行验证     

      if(CAN_receive_ADDR==ModuleConfig.CAN_ModuleAddress||CAN_receive_ADDR==0xc8)    //Is this our addresss?,广播地址
      {
        Checksum=0;
        for(i=0; i<8; i++) Checksum+=CANCmd[i];

        if(0==CANCmd[8])                              //Is the checksum correct?    //CAN 设备没有校验
        {
          ActualCommand.Opcode=CANCmd[0];
          ActualCommand.Type=CANCmd[1];
          ActualCommand.Motor=CANCmd[2];
          ActualCommand.Value.Byte[3]=CANCmd[3];
          ActualCommand.Value.Byte[2]=CANCmd[4];
          ActualCommand.Value.Byte[1]=CANCmd[5];
          ActualCommand.Value.Byte[0]=CANCmd[6];

          TMCLCommandState=TCS_CAN;                      //可以回复
          CAN_ReceiveFlag=0;

          //UARTCount=0;
        }
        else TMCLCommandState=TCS_CAN_ERROR;           //校验失败
      }
   }

   if(TMCLCommandState!=TCS_IDLE && TMCLCommandState!=TCS_CAN_ERROR)  ExecuteActualCommand();
   
}


/*******************************************************************************
* 函 数 名     : ExecuteActualCommand
* 函数功能		: 指令执行函数
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void ExecuteActualCommand(void)
   
{
  //Prepare answer
  ActualReply.Opcode=ActualCommand.Opcode;
  ActualReply.Status=REPLY_OK;
  ActualReply.Value.Int32=ActualCommand.Value.Int32;
   
  //Execute command
  switch(ActualCommand.Opcode)
  {
    case TMCL_ROR:
      RotateRight();
      break;

    case TMCL_ROL:
      RotateLeft();
      break;

    case TMCL_MST:
      MotorStop();
      break;

    case TMCL_MVP:
      MoveToPosition();
      break;

    case TMCL_SAP:
      SetAxisParameter();
      break;

    case TMCL_GAP:
      GetAxisParameter();
      break;
    
    case  TMC_STAP:
      Store_AxisParameter();
      break;
    case  TMC_RSAP:
      Restore_AxisParameter();
      break;
    
    case TMC_SGP:
      Set_GlobalParameter();
      break;    
    
    case TMC_GGP:
       Get_GlobalParameter();
      break;
    
    case TMC_RSGP:
       Restore_GlobalParameter();
      break;
    
    case TMC_RFS:
       Reference_search();
      break;
    
    case TMC_SIO:
       SIO_Output();
      break;
    
    case TMC_GIO:
       GIO_INput();
      break;
    
    
    
    case TMCL_GetVersion:
     // GetVersion();
      break;

    case TMCL_Boot:
     // Boot();
      break;

    case TMCL_SoftwareReset:
     // SoftwareReset();
      break;

    default:
      ActualReply.Status=REPLY_INVALID_CMD;
      break;
  }
   
}
/*******************************************************************************
* 函 数 名     : SetAxisParameter
* 函数功能		: 轴参数解析命令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void SetAxisParameter(void)
{   
   u8 Read[4], Write[4];
  // u8 MOTOR_NO=0;

 if(ActualCommand.Motor<NUM_Motor)
  {  
   switch(ActualCommand.Type)
   {
      case 0:
         Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //写目标位置
        break;
      
      case 1:
         Write429Int(IDX_XACTUAL|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //写当前位置
        break;
      
      case 2:
         Write429Int(IDX_VTARGET|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //写目标速度
        break;
      
      case 3:
         Write429Int(IDX_VACTUAL|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //写当前速度
        break;
      
      case 4:
         MotorConfig[ActualCommand.Motor].VMax=ActualCommand.Value.Int32;   
         Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].VMax);
      
        break;
      
      case 5:
         MotorConfig[ActualCommand.Motor].AMax=ActualCommand.Value.Int32;     
         Write429Short(IDX_AMAX|(ActualCommand.Motor<<5), MotorConfig[ActualCommand.Motor].AMax);        
         SetAMax(ActualCommand.Motor,MotorConfig[ActualCommand.Motor].AMax);    
        break;
      
      case 6:
         MotorConfig[ActualCommand.Motor].IRun=ActualCommand.Value.Byte[0];
         SetMotorCurrent(WHICH_262(ActualCommand.Motor), ActualCommand.Value.Byte[0]);    //262需要进行选择      
        break;

      case 7:
         MotorConfig[ActualCommand.Motor].IStandby=ActualCommand.Value.Byte[0];           //锁轴电流
        break;
      
      case 12:
         Write[0]=IDX_REFCONF_RM|TMC429_READ|(ActualCommand.Motor<<5);                    //右限位使能，失能
         ReadWrite429(Read, Write);
         Write[1]=Read[1];
         if(ActualCommand.Value.Byte[0]!=0)
           { 
              Write[2]=Read[2]|0x02;
              MotorConfig[ActualCommand.Motor].right_limit=1;
           }
         else
           { 
             Write[2]=Read[2]& ~0x02;
             MotorConfig[ActualCommand.Motor].right_limit=0;
           }
         Write[3]=Read[3];
         Write[0]=IDX_REFCONF_RM|(ActualCommand.Motor<<5);
         ReadWrite429(Read, Write);
        break;
         
      case 13:  
         Write[0]=IDX_REFCONF_RM|TMC429_READ|(ActualCommand.Motor<<5);                     //左限位开关使能，失能
         ReadWrite429(Read, Write);
         Write[1]=Read[1];
         if(ActualCommand.Value.Byte[0]!=0)
           {
            Write[2]=Read[2]|0x01;
            MotorConfig[ActualCommand.Motor].left_limit=1;
           }
         else
           {
           Write[2]=Read[2]&  ~0x01;
           MotorConfig[ActualCommand.Motor].left_limit=0;
           }
         Write[3]=Read[3];
         Write[0]=IDX_REFCONF_RM|(ActualCommand.Motor<<5);
         ReadWrite429(Read, Write);
        break;
      
      case 130:
         MotorConfig[ActualCommand.Motor].mini_speed=ActualCommand.Value.Int32;
         Write429Short(IDX_VMIN|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);       //设置最小速度
        break;      
           
      case 138:
         Set429RampMode(ActualCommand.Motor, ActualCommand.Value.Byte[0]);
        break;
      
      case 140:
         StepDirConfig[ActualCommand.Motor].MRes=8-ActualCommand.Value.Byte[0];      
         WriteStepDirConfig(ActualCommand.Motor);
        break;

      case 153:
         Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(ActualCommand.Motor<<5);
         ReadWrite429(Read, Write);
         Write[1]=Read[1];
         Write[2]=(Read[2] & 0xf0) | (ActualCommand.Value.Byte[0] & 0x0f);
         Write[3]=Read[3];
         Write[0]=IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5);
         ReadWrite429(Read, Write);
         MotorConfig[ActualCommand.Motor].RampDiv=ActualCommand.Value.Byte[0] & 0x0f;
        break;
         
      case 154:
         Write[0]=IDX_PULSEDIV_RAMPDIV|TMC429_READ|(ActualCommand.Motor<<5);
         ReadWrite429(Read, Write);
         Write[1]=Read[1];
         Write[2]=(Read[2] & 0x0f) | (ActualCommand.Value.Byte[0] << 4);
         Write[3]=Read[3];
         Write[0]=IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5);
         ReadWrite429(Read, Write);
         MotorConfig[ActualCommand.Motor].PulseDiv=ActualCommand.Value.Byte[0]& 0x0f;
        break;
      
      case 194:
         MotorConfig[ActualCommand.Motor].RFS_Speed=ActualCommand.Value.Int32;
        break;
      
      case 209:
          Set_Encoder_num(ActualCommand.Value.Int32) ;
         break;
      
      case 210:
         MotorConfig[ActualCommand.Motor].Encoder_pre=ActualCommand.Value.Int32;
        break;      
      
      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
   }
        
  } else ActualReply.Status=REPLY_INVALID_VALUE;
  
}
/*******************************************************************************
* 函 数 名     : SetAxisParameter
* 函数功能		: 轴参数解析命令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void GetAxisParameter(void)
{

  // u8 Read[4], Write[4];  

   if(ActualCommand.Motor<NUM_Motor)
   {
      ActualReply.Value.Int32=0;
       // MOTOR_NO=ActualCommand.Motor<<5;
    switch(ActualCommand.Type)
     {
       case 0:
          ActualReply.Value.Int32=Read429Int(IDX_XTARGET|(ActualCommand.Motor<<5));
         break;
       
       case 1:
          ActualReply.Value.Int32=Read429Int(IDX_XACTUAL|(ActualCommand.Motor<<5));
         break;
       
       case 2:
          ActualReply.Value.Int32=Read429Int(IDX_VTARGET|(ActualCommand.Motor<<5));
         break;
       
       case 3:
          ActualReply.Value.Int32=Read429Int(IDX_VACTUAL|(ActualCommand.Motor<<5));
         break;
       
       case 4:
          ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].VMax;
         break;
       
       case 5:
          ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].AMax;
         break;
       
       case 6:
          ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].IRun;
         break;    

       case 7:
          ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].IStandby;
         break;    
       
       case 8:
          if(Read429Status()>>(ActualCommand.Motor*2) & 0x01) ActualReply.Value.Byte[0]=1;       //回复是否到达
         break;
       
       case 9:
          if((Read429Status()>>(ActualCommand.Motor*2+1))&0x01)  ActualReply.Value.Byte[0]=1;     //回复限位开关状态
         break;
       
       case 10:
          ActualReply.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<ActualCommand.Motor*2)) ? 1:0;
         break;
       
        case 11:
          ActualReply.Value.Int32=(Read429SingleByte(IDX_REF_SWITCHES, 3) & (0x01<<ActualCommand.Motor*2+1)) ? 1:0;
         break;
        
        case 12:
           //////////////////////////
           
           ActualReply.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(ActualCommand.Motor<<5), 2) & 0x02) ? 1:0;
          break;

        case 13:
           /////////////////////////////
           
           ActualReply.Value.Byte[0]=(Read429SingleByte(IDX_REFCONF_RM|(ActualCommand.Motor<<5), 2) & 0x01) ? 1:0;
          break;
        
        case 130:
          //  ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].mini_speed;
           ActualReply.Value.Int32=Read429Short(IDX_VMIN|(ActualCommand.Motor<<5));
          break;
        
        case 135:
           ActualReply.Value.Int32=Read429Short(IDX_AACTUAL|(ActualCommand.Motor<<5));
          break;
        
        case 138:
           ActualReply.Value.Byte[0]=Read429SingleByte(IDX_REFCONF_RM|(ActualCommand.Motor<<5), 3);
          break;
        
        case 140:
           ActualReply.Value.Byte[0]=8-StepDirConfig[ActualCommand.Motor].MRes;
          break;
        
        case 141:
           ActualReply.Value.Int32=Read429Short(MOTOR_NUMBER(ActualCommand.Motor)<<5|IDX_DX_REFTOLERANCE);
          break;

        case 149:
           ActualReply.Value.Int32=(Read429SingleByte(IDX_REFCONF_RM|MOTOR_NUMBER(ActualCommand.Motor)<<5, 2) & 0x04) ? 1:0;
          break;
        
        case 153:
           ActualReply.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5), 2) & 0x0f;
          break;
        
        case 154:
           ActualReply.Value.Byte[0]=Read429SingleByte(IDX_PULSEDIV_RAMPDIV|(ActualCommand.Motor<<5), 2)>>4;
          break;
        
        case 194:
           ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].RFS_Speed;
        break;
        
        case 209:
           ActualReply.Value.Int32=Get_Encoder()*MotorConfig[ActualCommand.Motor].Encoder_pre/512;
        break;
        
        case 210:
           ActualReply.Value.Int32=MotorConfig[ActualCommand.Motor].Encoder_pre;
        break;
        
      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
     
  } else ActualReply.Status=REPLY_INVALID_VALUE;           
     
     
}

/*******************************************************************************
* 函 数 名     : Store_AxisParameter
* 函数功能		: 存储轴参数命令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Store_AxisParameter(void)
{

 if(ActualCommand.Motor<NUM_Motor)
  {   
   switch(ActualCommand.Type)
    {
      case 4:              //存储最大速度        
         WriteEepromBlock(0x100+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].VMax, 4);
        break;
      
      case 5:
         WriteEepromBlock(0x104+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].AMax, 4);
        break;
      
      case 6:
         WriteEepromByte(0x10A+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].IRun);
        break;
      
      case 7:
         WriteEepromByte(0x10B+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].IStandby);
        break;
      
      case 12:
         WriteEepromByte(0x120+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].right_limit);
        break;
      
      case 13:
         WriteEepromByte(0x121+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].left_limit);
        break;
         
      case 130:
         WriteEepromBlock(0x0118+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].mini_speed,3);
        break;
      
      case 140:
         WriteEepromByte(0x202+0x30*ActualCommand.Motor,StepDirConfig[ActualCommand.Motor].MRes);
       break;
      
      case 153:
         WriteEepromByte(0x109+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].RampDiv);
        break;
      
      case 154:
         WriteEepromByte(0x108+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].PulseDiv);
        break;
      
      case 194:        
         WriteEepromBlock(0x11c+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].RFS_Speed,3);
        break; 
      
      case 200:
         //无效，原因待查  ,已解决，eeprom 读写时间，将超时退出去掉即可正常工作。    
          
         WriteEepromByte(0x10,255);     //改变存储标志，进行全部更新
         Set429_EEPROM();               //更新EEprom
      
         WriteEepromByte(0x12,255);
         Set262_EEPROM();
       break;
      
      case 210:
         WriteEepromBlock(0x122+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].Encoder_pre,2);
        break;
      
     default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
     
  } else ActualReply.Status=REPLY_INVALID_VALUE;     

}
  
/*******************************************************************************
* 函 数 名     : Restore_AxisParameter
* 函数功能		: 恢复储轴参数命令 
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Restore_AxisParameter(void)
{
 if(ActualCommand.Motor<NUM_Motor)
  {   
   switch(ActualCommand.Type)
    {
      case 4:               //存储最大速度    
         ReadEepromBlock(0x0300,(u8*)&MotorConfig[ActualCommand.Motor].VMax,3);       
         WriteEepromBlock(0x100+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].VMax, 3);
        break;
      
      case 5:
         ReadEepromBlock(0x0304,(u8*)&MotorConfig[ActualCommand.Motor].AMax,3);
         WriteEepromBlock(0x104+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].AMax, 3);
        break;
      
      case 6:
         MotorConfig[ActualCommand.Motor].IRun=ReadEepromByte(0X30A);
         WriteEepromByte(0x10A+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].IRun);
        break;
      
      case 7:
         MotorConfig[ActualCommand.Motor].IStandby=ReadEepromByte(0X30B);
         WriteEepromByte(0x10B+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].IStandby);
        break;
      
      case 12:
         MotorConfig[ActualCommand.Motor].right_limit=ReadEepromByte(0X320);
         WriteEepromByte(0x120+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].right_limit);
        break;
      
      case 13:
         MotorConfig[ActualCommand.Motor].left_limit=ReadEepromByte(0X321);
         WriteEepromByte(0x121+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].left_limit);
        break;
         
      case 130:
         ReadEepromBlock(0x318,(u8*)&MotorConfig[ActualCommand.Motor].mini_speed,3);
         WriteEepromBlock(0x118+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].mini_speed,3);
        break;
      
      case 140:
         StepDirConfig[ActualCommand.Motor].MRes=ReadEepromByte(0X332);
         WriteEepromByte(0x202+0x30*ActualCommand.Motor,StepDirConfig[ActualCommand.Motor].MRes);
       break;
      
      case 153:
         MotorConfig[ActualCommand.Motor].RampDiv=ReadEepromByte(0X309);
         WriteEepromByte(0x109+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].RampDiv);
        break;
      
      case 154:
         MotorConfig[ActualCommand.Motor].PulseDiv=ReadEepromByte(0X308);
         WriteEepromByte(0x108+0x30*ActualCommand.Motor,MotorConfig[ActualCommand.Motor].PulseDiv);
        break;
      
      case 194:
         ReadEepromBlock(0x31c,(u8*)&MotorConfig[ActualCommand.Motor].RFS_Speed,3);       
         WriteEepromBlock(0x11c+0x30*ActualCommand.Motor,(u8*)&MotorConfig[ActualCommand.Motor].RFS_Speed,3);
        break; 
      
      case 200:                   //恢复全部轴参数，出厂设置
         //无效，原因待查  ,已解决，eeprom 读写时间，将超时退出去掉即可正常工作。    
          
         WriteEepromByte(0x10,255);    //改变存储标志，进行全部更新，此处为轴参数
         WriteEepromByte(0x12,255);
         
         Sys_Soft_Reset();             //系统重启
        
       break;
      
     default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
     
  } else ActualReply.Status=REPLY_INVALID_VALUE;     

}

/*******************************************************************************
* 函 数 名     : Set_GlobalParameter
* 函数功能		: 设置全局参数
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Set_GlobalParameter(void)
{

  switch(ActualCommand.Type)
    {
      case 69:                //can 波特率设置         
         ModuleConfig.CAN_Bitrate=ActualCommand.Value.Byte[0];
         WriteEepromByte(0x50,ModuleConfig.CAN_Bitrate);
         CAN_Init_ID();                                                         //CAN初始化环回模式,波特率500Kbps                  
        break;       
         
      case 70:
          ModuleConfig.CAN_ReplyAddress=ActualCommand.Value.Byte[0];           //回复ID
          WriteEepromByte(0x51,ModuleConfig.CAN_ReplyAddress);
          CAN_Init_ID();
        break;
      
      case 71:
          ModuleConfig.CAN_ModuleAddress=ActualCommand.Value.Byte[0];           //接收ID
          WriteEepromByte(0x52,ModuleConfig.CAN_ModuleAddress);
          CAN_Init_ID();
        break;  
     
     default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }   

}

/*******************************************************************************
* 函 数 名     : Get_GlobalParameter
* 函数功能		: 得到全局参数
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Get_GlobalParameter(void)
{
  switch(ActualCommand.Type)
    {
      case 69:                                   //can 波特率设置    
         ActualReply.Value.Byte[0]=ModuleConfig.CAN_Bitrate ;                  //CAN初始化环回模式,波特率500Kbps                  
        break;       
         
      case 70:
         ActualReply.Value.Byte[0]=ModuleConfig.CAN_ReplyAddress ;    
        break;
      
      case 71:
          ActualReply.Value.Byte[0]=ModuleConfig.CAN_ModuleAddress ;    
        break;  
     
     default:
         ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }   

}

/*******************************************************************************
* 函 数 名     : Restore_GlobalParameter
* 函数功能		: 回复全局参量
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Restore_GlobalParameter(void)
{   
    ModuleConfig.CAN_Bitrate=4 ;                  //CAN初始化环回模式,波特率500Kbps    
    ModuleConfig.CAN_ReplyAddress=3;    
    ModuleConfig.CAN_ModuleAddress=5 ;    
      
    CAN_Init_ID();
}

/*******************************************************************************
* 函 数 名     : RFS_RotateLeft
* 函数功能		: 开始寻位动作
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void RFS_RotateLeft(void)   
{
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_AMAX|(ActualCommand.Motor<<5), 1000);    //加速度  
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);    //最大速度     
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), -MotorConfig[ActualCommand.Motor].RFS_Speed);   //寻位速度
}

/*******************************************************************************
* 函 数 名     : Reference_search
* 函数功能		: 寻位指令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void Reference_search(void)   
{
   if(ActualCommand.Motor<NUM_Motor)
     {   
      switch(ActualCommand.Type)
       {
         case 0:                                             //开始进行寻位      
            RFS_FLAG[ActualCommand.Motor]=1;                 //开始寻位标志
            RFS_RotateLeft();                                //电机开始动作            
           break;
         
         case 1:            
            MotorStop();
            RFS_FLAG[ActualCommand.Motor]=5;                   //意外停止
           break;
         
         case 2:
            ActualReply.Value.Byte[0]=RFS_FLAG[ActualCommand.Motor];     //0是结束，其他是正在寻位
           break;
         
        default:
            ActualReply.Status=REPLY_WRONG_TYPE;
        break;

      }
   }   else ActualReply.Status=REPLY_INVALID_VALUE;   

}

/*******************************************************************************
* 函 数 名     : SIO_Output
* 函数功能		: IO 输出指令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void  SIO_Output(void)
{
 switch(ActualCommand.Type)
       {
         case 0:                                           //OpenDrain_1      
            if(ActualCommand.Value.Byte[0])
               PDout(5)=1;                                            //输出高
            else 
               PDout(5)=0;   
           break;
         
         case 1:            
           if(ActualCommand.Value.Byte[0])
               PDout(6)=1;                                            //输出高
            else 
               PDout(6)=0;   
           break;
         
         case 2:
            if(ActualCommand.Value.Byte[0])
               PDout(0)=1;                                            //输出高
            else 
               PDout(0)=0;   
           break;
            
         case 3:
            if(ActualCommand.Value.Byte[0])
               PDout(1)=1;                                            //输出高
            else 
               PDout(1)=0;   
           break;
            
         case 4:
            if(ActualCommand.Value.Byte[0])
               PDout(2)=1;                                            //输出高
            else 
               PDout(2)=0;   
           break;            
        
        default:
            ActualReply.Status=REPLY_WRONG_TYPE;
        break;

      }
       
}
/*******************************************************************************
* 函 数 名     : Reference_search
* 函数功能		: IO 输出指令
* 输    入     : 无
* 输    出     : 无
*******************************************************************************/
void  GIO_INput(void)
{
  u32 adcx;   
    switch(ActualCommand.Type)
       {
         case 0:                                           //OpenDrain_1    
            adcx=Get_Adc_Average(8,5);
            adcx=adcx*7370/4096;
            ActualReply.Value.Int32=adcx;    
           break;
         
         case 1:            
            ActualReply.Value.Byte[0]= PCin(0);    
           break;
         
         case 2:
            ActualReply.Value.Byte[0]= PCin(1);    
           break;
            
         case 3:
            ActualReply.Value.Byte[0]= PCin(2);    
           break;
                    
        default:
            ActualReply.Status=REPLY_WRONG_TYPE;
        break;

      }       
}



