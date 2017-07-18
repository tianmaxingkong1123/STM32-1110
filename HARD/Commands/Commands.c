/**********************************************************************
* Copyright (c) 2002-2017 ɽ�����������ҵ���޹�˾ All rights reserved.
*
* �ļ����ƣ� Commansds.c
* ��    ���� ָ����ļ���������ָ��Ľ������ָ����Լ���������
*
*
* �޸ļ�¼��
*
* V1.0��2006.12.22���Ǳ�
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
* �� �� ��     : PromcessCommands
* ��������		: ָ���ظ�
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void  PromcessCommands(void)
{
 
    u8 send_buf[8];
    u8 i;
    u8 Checksum;
   
   //����Ӧ��ָ��
   
   if(TMCLCommandState==TCS_CAN)  //via UART
  {
    if(TMCLReplyFormat==RF_STANDARD)             //������ʽ�ظ�����
    {
       /*
       Checksum=ModuleConfig.SerialHostAddress+ModuleConfig.SerialModuleAddress+
               ActualReply.Status+ActualReply.Opcode+
               ActualReply.Value.Byte[3]+
               ActualReply.Value.Byte[2]+
               ActualReply.Value.Byte[1]+
               ActualReply.Value.Byte[0];
      */
       //Can_Send_Msg((u8*)&ModuleConfig,8);//����8���ֽ�           
         
       //senf_buf[0]=ModuleConfig.SerialHostAddress;
       send_buf[0]=ModuleConfig.CAN_ModuleAddress; 
       send_buf[1]=ActualReply.Status;             //�����Ļ�Ϊ100
       send_buf[2]=ActualReply.Opcode;
       send_buf[3]=ActualReply.Value.Byte[3];
       send_buf[4]=ActualReply.Value.Byte[2];
       send_buf[5]=ActualReply.Value.Byte[1];
       send_buf[6]=ActualReply.Value.Byte[0];
       send_buf[7]=0;                             //Checksum        
 
       Can_Send_Msg(send_buf,8);
    }
    else if(TMCLReplyFormat==RF_SPECIAL)           //�����ʽ�ظ�����
    {
      for(i=0; i<9; i++)
      {
      //  WriteRS485(SpecialReply[i]);             //
      }
    }
  }
  
  else if(TMCLCommandState==TCS_CAN_ERROR)   //ָ�����ظ�
  {
       ActualReply.Opcode=0;
       ActualReply.Status=REPLY_CHKERR;     //״̬��Ϊ1  ��ʾ����
       ActualReply.Value.Int32=0;
     
    //У�����
   
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
     TMCLCommandState=TCS_IDLE;                   //��״̬
     TMCLReplyFormat=RF_STANDARD;                 //�����ظ�           


     if(ResetRequested) Sys_Soft_Reset();        //ϵͳ����

//**Try to get a new command**
  if(CAN_ReceiveFlag==1)     //Get data from UART
  {
     //�˴��Ƿ���Ҫ�ӳ�ʱ����ȵĴ�������Ҫ������֤     

      if(CAN_receive_ADDR==ModuleConfig.CAN_ModuleAddress||CAN_receive_ADDR==0xc8)    //Is this our addresss?,�㲥��ַ
      {
        Checksum=0;
        for(i=0; i<8; i++) Checksum+=CANCmd[i];

        if(0==CANCmd[8])                              //Is the checksum correct?    //CAN �豸û��У��
        {
          ActualCommand.Opcode=CANCmd[0];
          ActualCommand.Type=CANCmd[1];
          ActualCommand.Motor=CANCmd[2];
          ActualCommand.Value.Byte[3]=CANCmd[3];
          ActualCommand.Value.Byte[2]=CANCmd[4];
          ActualCommand.Value.Byte[1]=CANCmd[5];
          ActualCommand.Value.Byte[0]=CANCmd[6];

          TMCLCommandState=TCS_CAN;                      //���Իظ�
          CAN_ReceiveFlag=0;

          //UARTCount=0;
        }
        else TMCLCommandState=TCS_CAN_ERROR;           //У��ʧ��
      }
   }

   if(TMCLCommandState!=TCS_IDLE && TMCLCommandState!=TCS_CAN_ERROR)  ExecuteActualCommand();
   
}


/*******************************************************************************
* �� �� ��     : ExecuteActualCommand
* ��������		: ָ��ִ�к���
* ��    ��     : ��
* ��    ��     : ��
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
* �� �� ��     : SetAxisParameter
* ��������		: �������������
* ��    ��     : ��
* ��    ��     : ��
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
         Write429Int(IDX_XTARGET|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //дĿ��λ��
        break;
      
      case 1:
         Write429Int(IDX_XACTUAL|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //д��ǰλ��
        break;
      
      case 2:
         Write429Int(IDX_VTARGET|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //дĿ���ٶ�
        break;
      
      case 3:
         Write429Int(IDX_VACTUAL|(ActualCommand.Motor<<5),ActualCommand.Value.Int32);   //д��ǰ�ٶ�
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
         SetMotorCurrent(WHICH_262(ActualCommand.Motor), ActualCommand.Value.Byte[0]);    //262��Ҫ����ѡ��      
        break;

      case 7:
         MotorConfig[ActualCommand.Motor].IStandby=ActualCommand.Value.Byte[0];           //�������
        break;
      
      case 12:
         Write[0]=IDX_REFCONF_RM|TMC429_READ|(ActualCommand.Motor<<5);                    //����λʹ�ܣ�ʧ��
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
         Write[0]=IDX_REFCONF_RM|TMC429_READ|(ActualCommand.Motor<<5);                     //����λ����ʹ�ܣ�ʧ��
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
         Write429Short(IDX_VMIN|(ActualCommand.Motor<<5), ActualCommand.Value.Int32);       //������С�ٶ�
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
* �� �� ��     : SetAxisParameter
* ��������		: �������������
* ��    ��     : ��
* ��    ��     : ��
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
          if(Read429Status()>>(ActualCommand.Motor*2) & 0x01) ActualReply.Value.Byte[0]=1;       //�ظ��Ƿ񵽴�
         break;
       
       case 9:
          if((Read429Status()>>(ActualCommand.Motor*2+1))&0x01)  ActualReply.Value.Byte[0]=1;     //�ظ���λ����״̬
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
* �� �� ��     : Store_AxisParameter
* ��������		: �洢���������
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Store_AxisParameter(void)
{

 if(ActualCommand.Motor<NUM_Motor)
  {   
   switch(ActualCommand.Type)
    {
      case 4:              //�洢����ٶ�        
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
         //��Ч��ԭ�����  ,�ѽ����eeprom ��дʱ�䣬����ʱ�˳�ȥ����������������    
          
         WriteEepromByte(0x10,255);     //�ı�洢��־������ȫ������
         Set429_EEPROM();               //����EEprom
      
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
* �� �� ��     : Restore_AxisParameter
* ��������		: �ָ������������ 
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Restore_AxisParameter(void)
{
 if(ActualCommand.Motor<NUM_Motor)
  {   
   switch(ActualCommand.Type)
    {
      case 4:               //�洢����ٶ�    
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
      
      case 200:                   //�ָ�ȫ�����������������
         //��Ч��ԭ�����  ,�ѽ����eeprom ��дʱ�䣬����ʱ�˳�ȥ����������������    
          
         WriteEepromByte(0x10,255);    //�ı�洢��־������ȫ�����£��˴�Ϊ�����
         WriteEepromByte(0x12,255);
         
         Sys_Soft_Reset();             //ϵͳ����
        
       break;
      
     default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
     
  } else ActualReply.Status=REPLY_INVALID_VALUE;     

}

/*******************************************************************************
* �� �� ��     : Set_GlobalParameter
* ��������		: ����ȫ�ֲ���
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Set_GlobalParameter(void)
{

  switch(ActualCommand.Type)
    {
      case 69:                //can ����������         
         ModuleConfig.CAN_Bitrate=ActualCommand.Value.Byte[0];
         WriteEepromByte(0x50,ModuleConfig.CAN_Bitrate);
         CAN_Init_ID();                                                         //CAN��ʼ������ģʽ,������500Kbps                  
        break;       
         
      case 70:
          ModuleConfig.CAN_ReplyAddress=ActualCommand.Value.Byte[0];           //�ظ�ID
          WriteEepromByte(0x51,ModuleConfig.CAN_ReplyAddress);
          CAN_Init_ID();
        break;
      
      case 71:
          ModuleConfig.CAN_ModuleAddress=ActualCommand.Value.Byte[0];           //����ID
          WriteEepromByte(0x52,ModuleConfig.CAN_ModuleAddress);
          CAN_Init_ID();
        break;  
     
     default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }   

}

/*******************************************************************************
* �� �� ��     : Get_GlobalParameter
* ��������		: �õ�ȫ�ֲ���
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Get_GlobalParameter(void)
{
  switch(ActualCommand.Type)
    {
      case 69:                                   //can ����������    
         ActualReply.Value.Byte[0]=ModuleConfig.CAN_Bitrate ;                  //CAN��ʼ������ģʽ,������500Kbps                  
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
* �� �� ��     : Restore_GlobalParameter
* ��������		: �ظ�ȫ�ֲ���
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Restore_GlobalParameter(void)
{   
    ModuleConfig.CAN_Bitrate=4 ;                  //CAN��ʼ������ģʽ,������500Kbps    
    ModuleConfig.CAN_ReplyAddress=3;    
    ModuleConfig.CAN_ModuleAddress=5 ;    
      
    CAN_Init_ID();
}

/*******************************************************************************
* �� �� ��     : RFS_RotateLeft
* ��������		: ��ʼѰλ����
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void RFS_RotateLeft(void)   
{
    SpeedChangedFlag[ActualCommand.Motor]=TRUE;
    Set429RampMode(ActualCommand.Motor, RM_VELOCITY);
    Write429Short(IDX_AMAX|(ActualCommand.Motor<<5), 1000);    //���ٶ�  
    Write429Short(IDX_VMAX|(ActualCommand.Motor<<5), 2047);    //����ٶ�     
    Write429Short(IDX_VTARGET|(ActualCommand.Motor<<5), -MotorConfig[ActualCommand.Motor].RFS_Speed);   //Ѱλ�ٶ�
}

/*******************************************************************************
* �� �� ��     : Reference_search
* ��������		: Ѱλָ��
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void Reference_search(void)   
{
   if(ActualCommand.Motor<NUM_Motor)
     {   
      switch(ActualCommand.Type)
       {
         case 0:                                             //��ʼ����Ѱλ      
            RFS_FLAG[ActualCommand.Motor]=1;                 //��ʼѰλ��־
            RFS_RotateLeft();                                //�����ʼ����            
           break;
         
         case 1:            
            MotorStop();
            RFS_FLAG[ActualCommand.Motor]=5;                   //����ֹͣ
           break;
         
         case 2:
            ActualReply.Value.Byte[0]=RFS_FLAG[ActualCommand.Motor];     //0�ǽ���������������Ѱλ
           break;
         
        default:
            ActualReply.Status=REPLY_WRONG_TYPE;
        break;

      }
   }   else ActualReply.Status=REPLY_INVALID_VALUE;   

}

/*******************************************************************************
* �� �� ��     : SIO_Output
* ��������		: IO ���ָ��
* ��    ��     : ��
* ��    ��     : ��
*******************************************************************************/
void  SIO_Output(void)
{
 switch(ActualCommand.Type)
       {
         case 0:                                           //OpenDrain_1      
            if(ActualCommand.Value.Byte[0])
               PDout(5)=1;                                            //�����
            else 
               PDout(5)=0;   
           break;
         
         case 1:            
           if(ActualCommand.Value.Byte[0])
               PDout(6)=1;                                            //�����
            else 
               PDout(6)=0;   
           break;
         
         case 2:
            if(ActualCommand.Value.Byte[0])
               PDout(0)=1;                                            //�����
            else 
               PDout(0)=0;   
           break;
            
         case 3:
            if(ActualCommand.Value.Byte[0])
               PDout(1)=1;                                            //�����
            else 
               PDout(1)=0;   
           break;
            
         case 4:
            if(ActualCommand.Value.Byte[0])
               PDout(2)=1;                                            //�����
            else 
               PDout(2)=0;   
           break;            
        
        default:
            ActualReply.Status=REPLY_WRONG_TYPE;
        break;

      }
       
}
/*******************************************************************************
* �� �� ��     : Reference_search
* ��������		: IO ���ָ��
* ��    ��     : ��
* ��    ��     : ��
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



