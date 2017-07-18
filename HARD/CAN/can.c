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
#include "can.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Commands.h"
#include "AT25128B.h"


/*******************************************************************************
* �� �� ��     : SystemControl()
* ��������		: CAN��ʼ��
* ��    ��     : ��
* ��    ��     : ��
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;  tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
*******************************************************************************/

u8 can_usmart_flag;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
   
{
   GPIO_InitTypeDef 		   GPIO_InitStructure; 
	CAN_InitTypeDef        	CAN_InitStructure;
//	CAN_FilterInitTypeDef  	CAN_FilterInitStructure;
   
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  		NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//ʹ��PORTAʱ��	      

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��
  
   GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			      //��ʼ��IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	   //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			      //��ʼ��IO

	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM=DISABLE;			         //��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM=DISABLE;			         //�����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			         //˯��ģʽͨ����������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;			         //��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	         //���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP=DISABLE;			         //���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode= mode;	               //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; 
	//���ò�����
	CAN_InitStructure.CAN_SJW=tsjw;				         //����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			            //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				         //Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;               //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	      //��ʼ��CAN1 
   /*
	CAN_FilterInitStructure.CAN_FilterNumber=2;	                     //������0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	   //����λģʽ
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	//32λ�� 
	CAN_FilterInitStructure.CAN_FilterIdHigh=(0x0005<<5);	               //32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=(0x00c8<<5);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x00FF<<5;              //32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x00FF<<5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;              //���������0
   */
	//CAN_FilterInit(&CAN_FilterInitStructure);			                  //�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE 

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				                  //FIFO0��Ϣ�Һ��ж�����.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;         // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;      
}

#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	int i=0;
   CAN_Receive(CAN1, 0, &RxMessage);
   
	for(i=0;i<8;i++)
   {
	//printf("rxbuf:%d  ",RxMessage.Data[i]);
   CANCmd[i]=RxMessage.Data[i];
   }
  // printf("%d\r\n",RxMessage.StdId);			// ��׼��ʶ��
   
   CAN_receive_ADDR=RxMessage.StdId;        //���͵��豸��ַ
   CAN_ReceiveFlag=1;
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 Can_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=ModuleConfig.CAN_ReplyAddress;			   // ��׼��ʶ�� 
	TxMessage.ExtId=ModuleConfig.CAN_ReplyAddress;			   // ������չ��ʾ�� 
	TxMessage.IDE=CAN_Id_Standard; 	// ��׼֡
	TxMessage.RTR=CAN_RTR_Data;		// ����֡
	TxMessage.DLC=len;				   // Ҫ���͵����ݳ���
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];	
   
   /***************���ô��ڷ���******************/
   if(can_usmart_flag==1)
   {
     printf("RX_ID:%0.2X  DATA:  ",ModuleConfig.CAN_ReplyAddress);
   for(i=0;i<8;i++)
   {
	printf("%0.2X  ",msg[i]);
   }
   printf("\r\n");
   can_usmart_flag=0;
   }
   /*******************************************/
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0;    
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
	if(i>=0XFFF)return 1;
	return 0;	 
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}


/****************************************************************************
* Function Name  : CAN1_Config16BitFilter
* Description    : ����CAN����16������׼ID������IDλ��ȫ����ͬ���ܹ�ͨ����
* Input          : id1��Ҫ���յ�һ��ID
*                * id2��Ҫ���յ�һ��ID
* Output         : None 
* Return         : None
****************************************************************************/

void CAN1_Config16BitFilter(u16 id1, u16 id2)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    u16 mask = 0xFFFF;

	/* CAN filter init ���μĴ�����ʼ�� */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;	               //������1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//IDģʽ

	/* �Ĵ���������Ϊ16λ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
   CAN_FilterInitStructure.CAN_FilterIdHigh = (id1 << 5);     //Ҫ���յ�ID��ʾ��1		
   CAN_FilterInitStructure.CAN_FilterIdLow =  (id2 << 5);	  //Ҫ���յ�ID��ʾ��2

	/* ����Ϊ����IDλ��Ҫ��ͬ�Ž��� */	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (mask << 5);  //MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = (mask << 5);
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;      //ʹ�ܹ�����1

	CAN_FilterInit(&CAN_FilterInitStructure);
}


void CAN_Init_ID(void)
{
      u16 brp;
   
     if(ReadEepromByte(0x14)!=55)
      { 
         WriteEepromByte(0x50,ModuleConfig.CAN_Bitrate);
         WriteEepromByte(0x51,ModuleConfig.CAN_ReplyAddress);
         WriteEepromByte(0x52,ModuleConfig.CAN_ModuleAddress);  
         
         WriteEepromByte(0x14,55);
      }
       ModuleConfig.CAN_Bitrate= ReadEepromByte(0x50);
       ModuleConfig.CAN_ReplyAddress=ReadEepromByte(0x51);
       ModuleConfig.CAN_ModuleAddress=ReadEepromByte(0x52);
        switch(ModuleConfig.CAN_Bitrate)
         {
            case 0:  brp=400;   break;   //   5          
            case 1:  brp=200;   break;   //   10     
            case 2:  brp=100;   break;   //   20
            case 3:  brp=40;    break;   //   50
            case 4:  brp=20;    break;   //   100
            case 5:  brp=16;    break;   //   125
            case 6:  brp=8;     break;   //   250
            case 7:  brp=4;     break;   //   500
            case 8:  brp=2;     break;   //   1000    
          default:   
                break;          
         }
         
      CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,brp,CAN_Mode_Normal);   //CAN��ʼ������ģʽ,������500Kbps  
         
      CAN1_Config16BitFilter(ModuleConfig.CAN_ModuleAddress,200);
}
   

void Can_usart(u8 id,u8 buf0,u8 buf1,u8 buf2,u32 buf3)
{
   u8 i;
   CANCmd[0]=buf0;
   CANCmd[1]=buf1;
   CANCmd[2]=buf2;
   CANCmd[3]=buf3>>24;
   CANCmd[4]=buf3>>16;
   CANCmd[5]=buf3>>8;
   CANCmd[6]=buf3&0x000000ff;
   CANCmd[7]=0;   
   
   printf("\r\nTX_ID:%0.2X  DATA:  ",id);
   for(i=0;i<8;i++)
   {
	 printf("%0.2X  ",CANCmd[i]);
   }
   
   CAN_receive_ADDR=id;
   CAN_ReceiveFlag=1;
   can_usmart_flag=1;               //���ڵ�CAN �Ĳ������ݣ����ô��ڻظ�

}




