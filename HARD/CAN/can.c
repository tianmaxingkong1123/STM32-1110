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
#include "can.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "Commands.h"
#include "AT25128B.h"


/*******************************************************************************
* 函 数 名     : SystemControl()
* 函数功能		: CAN初始化
* 输    入     : 无
* 输    出     : 无
//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:时间段2的时间单元.   范围:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:时间段1的时间单元.   范围:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :波特率分频器.范围:1~1024;  tq=(brp)*tpclk1
//波特率=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,普通模式;CAN_Mode_LoopBack,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//返回值:0,初始化OK;
//    其他,初始化失败; 
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

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//使能PORTA时钟	      

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟
  
   GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   //复用推挽
	GPIO_Init(GPIOB, &GPIO_InitStructure);			      //初始化IO

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	   //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);			      //初始化IO

	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;			         //非时间触发通信模式  
	CAN_InitStructure.CAN_ABOM=DISABLE;			         //软件自动离线管理	 
	CAN_InitStructure.CAN_AWUM=DISABLE;			         //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
	CAN_InitStructure.CAN_NART=ENABLE;			         //禁止报文自动传送 
	CAN_InitStructure.CAN_RFLM=DISABLE;		 	         //报文不锁定,新的覆盖旧的  
	CAN_InitStructure.CAN_TXFP=DISABLE;			         //优先级由报文标识符决定 
	CAN_InitStructure.CAN_Mode= mode;	               //模式设置： mode:0,普通模式;1,回环模式; 
	//设置波特率
	CAN_InitStructure.CAN_SJW=tsjw;				         //重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1=tbs1; 			            //Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2=tbs2;				         //Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler=brp;               //分频系数(Fdiv)为brp+1	
	CAN_Init(CAN1, &CAN_InitStructure);        	      //初始化CAN1 
   /*
	CAN_FilterInitStructure.CAN_FilterNumber=2;	                     //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 	   //屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit; 	//32位宽 
	CAN_FilterInitStructure.CAN_FilterIdHigh=(0x0005<<5);	               //32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=(0x00c8<<5);
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x00FF<<5;              //32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x00FF<<5;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;              //激活过滤器0
   */
	//CAN_FilterInit(&CAN_FilterInitStructure);			                  //滤波器初始化
	
#if CAN_RX0_INT_ENABLE 

	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				                  //FIFO0消息挂号中断允许.		    

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;         // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;      
}

#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
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
  // printf("%d\r\n",RxMessage.StdId);			// 标准标识符
   
   CAN_receive_ADDR=RxMessage.StdId;        //发送的设备地址
   CAN_ReceiveFlag=1;
}
#endif

//can发送一组数据(固定格式:ID为0X12,标准帧,数据帧)	
//len:数据长度(最大为8)				     
//msg:数据指针,最大为8个字节.
//返回值:0,成功;
//		 其他,失败;
u8 Can_Send_Msg(u8* msg,u8 len)
{	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=ModuleConfig.CAN_ReplyAddress;			   // 标准标识符 
	TxMessage.ExtId=ModuleConfig.CAN_ReplyAddress;			   // 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;		// 数据帧
	TxMessage.DLC=len;				   // 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];	
   
   /***************启用串口发送******************/
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
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for(i=0;i<8;i++)
    buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}


/****************************************************************************
* Function Name  : CAN1_Config16BitFilter
* Description    : 设置CAN接收16两个标准ID（设置ID位数全部相同才能够通过）
* Input          : id1：要接收的一个ID
*                * id2：要接收的一个ID
* Output         : None 
* Return         : None
****************************************************************************/

void CAN1_Config16BitFilter(u16 id1, u16 id2)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    u16 mask = 0xFFFF;

	/* CAN filter init 屏蔽寄存器初始化 */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;	               //过滤器1
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;//ID模式

	/* 寄存器组设置为16位 */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_16bit;
   CAN_FilterInitStructure.CAN_FilterIdHigh = (id1 << 5);     //要接收的ID标示符1		
   CAN_FilterInitStructure.CAN_FilterIdLow =  (id2 << 5);	  //要接收的ID标示符2

	/* 设置为所有ID位都要相同才接收 */	
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = (mask << 5);  //MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow  = (mask << 5);
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0; //FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;      //使能过滤器1

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
         
      CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,brp,CAN_Mode_Normal);   //CAN初始化环回模式,波特率500Kbps  
         
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
   can_usmart_flag=1;               //串口到CAN 的参数传递，启用串口回复

}





